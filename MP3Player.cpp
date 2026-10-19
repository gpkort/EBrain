#include "HardwareSerial.h"
#include <vector>
#include <exception>
#include "WString.h"
#include "MP3Player.h"

namespace MP3Control
{
   MP3Player::MP3Player(int8_t rx, int8_t tx)
    {
      BT201Serial.setRxBufferSize(256);
      BT201Serial.begin(BT201_BAUD_RATE, BT201_CONFIGURATION, rx, tx);
      mSerialMutex = xSemaphoreCreateBinary();
      mPlayingMutex = xSemaphoreCreateBinary();
      if(mSerialMutex != NULL) { xSemaphoreGive(mSerialMutex); } //
      if(mPlayingMutex != NULL) { xSemaphoreGive(mPlayingMutex); }
    }

    MP3Player::~MP3Player(){}
    
    void MP3Player::sendBT201CommandStr(String cmd, bool sendEOL) {   
      cmd = String(BT201Commands::AT_COMMAND) + cmd;   
      if(sendEOL)
      {
        cmd += BT201Commands::ENDING;
      }
      Serial.println("Sending " + cmd);
      BT201Serial.print(cmd.c_str());      
    }

    void MP3Player::sendBT201Command(const char* cmd) {
      Serial.println("Command: " + String(cmd));
      BT201Serial.print(BT201Commands::AT_COMMAND);
      BT201Serial.print(cmd);
      BT201Serial.print(BT201Commands::ENDING);      
    }

    String MP3Player::queryBT201(String cmd, bool sendEOL) {
      sendBT201CommandStr(cmd, sendEOL);
      return getResponse();
    }

    String MP3Player::queryBT201(const char* cmd) {
      sendBT201Command(cmd);
      return getResponse();
    }

    bool MP3Player::setPlayerMode() {
      queryBT201(BT201Commands::USE_SD_CARD);
      delayMicroseconds(200);
      auto res = queryBT201(BT201Commands::QUERY_MODE);
      res.trim();
      return res.startsWith(BT201Responses::SD_CARD_MUSIC_MODE);
    }

    void MP3Player::setSongPlayingMode() {
      auto res = queryBT201(BT201Commands::PICK_SONG_MODE);
    }

    bool MP3Player::stopPlayer() {
      queryBT201(BT201Commands::STOP_SONG);
      delay(200);
      auto res = queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
      return  res.startsWith(BT201Responses::RESPONSE_MUSIC_STATE_STOPPED);
    }

    void MP3Player::initBT201()
    {  
      delay(1000);
      auto res = queryBT201(BT201Commands::QUERY_MODE);
      Serial.println(res);    
      // setPlayerMode();     
      // mMode == PlayerMode::MUSIC;
      // delay(200);
      // play_pause();
      // mMusicState = MusicState::STOPPED;
      // delay(200);
      // setSongPlayingMode();  
      // Serial.println("SETUP Complete");
             
    }
        
    //Song Operations
    void MP3Player::playAction(SongPlay::PlayMessage message) {
        std::vector<int> songs;
        switch (message.action) {
          case SongPlay::Action::PLAY_ALL:
            Serial.println("Play All");            
            if(message.startIdx > -1 && message.endIdx > message.startIdx > -1)
            {
              for(int i=message.startIdx;i<=message.endIdx;i++){songs.push_back(i);}              
              playSongs(songs);
            }
            return;
          case SongPlay::Action::PLAY_SINGLE:
            Serial.printf("Single: %d\n", message.startIdx); 
            songs = {message.startIdx};
            playSongs(songs);
            return;
          case SongPlay::Action::SHUFFLE:
            Serial.println("Shuffle: ");
            // TODO: implement
            return;
          case SongPlay::Action::UNSET:
          default:
            Serial.println("No Action");     
            return;
          }
    }

    void MP3Player::play_pause()  {
      sendBT201Command(BT201Commands::PP_SONG);
    }    

    void MP3Player::previousSong()
    {
      sendBT201Command(BT201Commands::PREVIOUS_SONG);
    }

    void MP3Player::nextSong()  {
      sendBT201Command(BT201Commands::NEXT_SONG);
    }

    void MP3Player::playSong(int index) {
      Serial.printf("Play: %d\n", index); 
      auto res = queryBT201(BT201Commands::PLAY_SPECIFIC_INDEX + String(index));
      Serial.println("start playing: " + res);
      delay(250);
      res = queryBT201(BT201Commands::QUERY_FILE_NUM);
      Serial.println("is it playing: " + res);
    }

    void MP3Player::playSongs(std::vector<int>& songs) {
      if (xSemaphoreTake(this->mPlayingMutex, 500))
      {
        if(mMusicState != MusicState::PLAYING && mMonitorTask == NULL) {
          mSongMap.clear();
          for(int i=songs.front(); i<=songs.front();i++) {mSongMap[i]=false;}
          auto one = mSongMap.begin();
          one->second = true;
          auto res = queryBT201(String(BT201Commands::PLAY_SPECIFIC_INDEX) + String(one->first));          
          res = queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
          
          if(res.startsWith(BT201Responses::RESPONSE_MUSIC_STATE_PLAYING)) {
            Serial.println("Start listening");
            // delay(1000);          
            mMusicState == MusicState::PLAYING;
            xSemaphoreGive(mPlayingMutex);
            xTaskCreatePinnedToCore(MP3Player::taskWrapper, "Monitor", 10000, NULL, 
                                    1, &mMonitorTask, 0);
            return;
          }
        }
        xSemaphoreGive(mPlayingMutex);
      }
    }

    void MP3Player::volumeUp() {
      sendBT201Command(BT201Commands::RAISE_VOLUME);
      ++mCurrentVolume;
    }

    void MP3Player::volumeDown() {
      sendBT201Command(BT201Commands::LOWER_VOLUME);
      --mCurrentVolume;
    }
    
    void MP3Player::setVolume(int level) {
      if(level >= VOLUME_MIN and level <= VOLUME_MAX)
      {
        String comm = String(BT201Commands::SET_VOLUME) + String(level);
        sendBT201Command(comm.c_str());
        mCurrentVolume = level;
      }
    }
            
    String MP3Player::getResponse(int timeout) {
      String serialString = "";
      
      if (xSemaphoreTake(mSerialMutex, 1000)) 
      {
        int start = millis();
        while((millis() - start) < timeout)
        {
          if(BT201Serial.available() > 0)
          {
            serialString = BT201Serial.readString();
            break;
          }
          delayMicroseconds(1000);
        }
        clearSerialBuffer();
        xSemaphoreGive(mSerialMutex);        
      }
           
      return serialString;
    }



    void taskWrapper(void* _o) {
      Serial.println("Called");
      MP3Player* self = static_cast<MP3Player*>(_o);
        for(;;) {
          auto sem = self->getPlayingMutex();
          if(xSemaphoreTake(sem, 500)) {
          }
          vTaskDelay(pdMS_TO_TICKS(500));
      }
    }

    void MP3Player::monitorPlaying() {
      for(;;) {        
        if(xSemaphoreTake(mPlayingMutex, 500)) {
        //   auto res = queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
        //   if(res == String(BT201Responses::RESPONSE_MUSIC_STATE_PLAYING)) {
        //     PlayStatus::SongStatus status;
        //     //song index
        //     res = queryBT201(BT201Commands::QUERY_FILE_NUM);
        //     if(res.startsWith(BT201Responses::RESPONSE_FILE_NUM)) {
        //       status.index = (int) strtol(res.substring(3).c_str(), NULL, 16);
        //     }

        //     //total time
        //     res = queryBT201(BT201Commands::QUERY_PLAYBACK_TOTAL_TIME);
        //     if(res.startsWith(BT201Responses::RESPONSE_PLAYBACK_TOTAL_TIME)) {
        //       status.totalTime = (int) strtol(res.substring(3).c_str(), NULL, 16);
        //     }

        //     //total time
        //     res = queryBT201(BT201Commands::QUERY_PLAYBACK_PLAYING_TIME);
        //     if(res.startsWith(BT201Responses::RESPONSE_PLAYBACK_PLAYING_TIME)) {
        //       status.totalTime = (int) strtol(res.substring(3).c_str(), NULL, 16);
        //     }
        //     Serial.println(status.toString());            
        //   }
          // else if(res == String(BT201Responses::RESPONSE_MUSIC_STATE_STOPPED)) {
          //   Serial.println("The song list is over");
          //   xSemaphoreGive(mPlayingMutex);
          //   vTaskDelete(mMonitorTask);
          //   mMonitorTask = NULL;
          // }
          // vTaskDelay(pdMS_TO_TICKS(500)); // Delay 1 second
          // xSemaphoreGive(mPlayingMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
      }
    }

    void MP3Player::clearSerialBuffer() {
      while(BT201Serial.available()){BT201Serial.read();}
    }

} //MP3Contro