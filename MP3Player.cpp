#include "esp32-hal.h"
#include "HardwareSerial.h"
#include <vector>
#include <algorithm> 
#include <random>
#include "WString.h"
#include "MP3Player.h"


MP3Player::MP3Player(int8_t rx, int8_t tx) {
    BT201Serial.setRxBufferSize(256);
    BT201Serial.begin(BT201_BAUD_RATE, BT201_CONFIGURATION, rx, tx);
    mSerialMutex = xSemaphoreCreateBinary();
    mPlayingMutex = xSemaphoreCreateBinary();
    if(mSerialMutex != NULL) { xSemaphoreGive(mSerialMutex); } //
    if(mPlayingMutex != NULL) { xSemaphoreGive(mPlayingMutex); }

    if(setPlayerReady())
    {
      setMusicState(MusicState::IDLE);
      mMode = PlayerMode::MUSIC;
      xTaskCreatePinnedToCore(MP3Player::worker, "worker", 10000, this, 1, &mWorkTask, 0);
    }      
}

MP3Player::~MP3Player(){}
  
void MP3Player::sendBT201Command(String cmd, bool sendEOL) {   
  cmd = String(BT201Commands::AT_COMMAND) + cmd;   
  if(sendEOL)
  {
    cmd += BT201Commands::ENDING;
  }
  BT201Serial.print(cmd.c_str());      
}

void MP3Player::sendBT201Command(const char* cmd) {
  //Serial.println("Command: " + String(cmd));
  BT201Serial.print(BT201Commands::AT_COMMAND);
  BT201Serial.print(cmd);
  BT201Serial.print(BT201Commands::ENDING);      
}

String MP3Player::queryBT201(String cmd, bool sendEOL) {
  sendBT201Command(cmd, sendEOL);
  return getResponse();
}

String MP3Player::queryBT201(const char* cmd) {
  sendBT201Command(cmd);
  return getResponse();
}

  bool MP3Player::setPlayerReady() {
    queryBT201(BT201Commands::USE_SD_CARD);
    delayMicroseconds(200);
    auto res = queryBT201(BT201Commands::QUERY_MODE);
    res.trim();
    return res.startsWith(BT201Responses::SD_CARD_MUSIC_MODE);
  }

  void MP3Player::MP3Player::setMusicState(MusicState state) {
    if (xSemaphoreTake(this->mPlayingMutex, 750))
    {
        mMusicState = state;
        xSemaphoreGive(mPlayingMutex);
    }
  }

  MusicState MP3Player::getMusicState() {
    MusicState state
    if (xSemaphoreTake(this->mPlayingMutex, 750)) {
        state = mMusicState;
        xSemaphoreGive(mPlayingMutex);
    }
    return state;
  }
  
  bool MP3Player::stopPlayer() {
    queryBT201(BT201Commands::STOP_SONG);
    delay(200);
    auto res = queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
    return  res.startsWith(BT201Responses::RESPONSE_MUSIC_STATE_STOPPED);
  }
          
  //Song Operations
  void MP3Player::playActionCB(Event::CommandEvent commandEvent) {  
    mCommandQueue.push(commandEvent);
  }

  void MP3Player::play_pause()  {
    sendBT201Command(BT201Commands::PP_SONG);
  }    

  void MP3Player::previousSong() {
    sendBT201Command(BT201Commands::PREVIOUS_SONG);
  }

  void MP3Player::nextSong()  {
    sendBT201Command(BT201Commands::NEXT_SONG);
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

  bool MP3Player::playNextSong() {
    bool ret  = false;
    if(!mSongQueue.empty()) {
      auto res1 = queryBT201(String(BT201Commands::PLAY_SPECIFIC_INDEX) + String(mSongQueue.front()));
      auto start = res1.indexOf(BT201Commands::QUERY_PLAYBACK_TOTAL_TIME);
      if(start != -1) {            
        auto end = res1.indexOf("\n", start);
        if(end != -1) {
          auto mts = res1.substring(start, end);
          mCurrentSongTotalTime = (int) strtol(mts.substring(3).c_str(), NULL, 16);
        }
      }
      auto res = queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
      if(res.startsWith(BT201Responses::RESPONSE_MUSIC_STATE_PLAYING)) {
        mSongQueue.pop();
        ret = true;
      }        
    }
    return ret;
  }
          
  String MP3Player::getResponse(int timeout) {
    String serialString = "";
    
    if (xSemaphoreTake(mSerialMutex, 1000)) 
    {
      long start = millis();
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

  PlayStatus::SongStatus MP3Player::updateStatus() {
    PlayStatus::SongStatus status;
    //song index
    auto res = queryBT201(BT201Commands::QUERY_FILE_NUM);
    if(res.startsWith(BT201Responses::RESPONSE_FILE_NUM)) {
      status.index = (int) strtol(res.substring(3).c_str(), NULL, 16);
    }

    //playing time
    res = queryBT201(BT201Commands::QUERY_PLAYBACK_PLAYING_TIME);
    if(res.startsWith(BT201Responses::RESPONSE_PLAYBACK_PLAYING_TIME)) {
      status.playingTime = (int) strtol(res.substring(3).c_str(), NULL, 16);
    }
    status.totalTime = mCurrentSongTotalTime;

    return status;
  }

  void MP3Player::worker(void* _o) {
    MP3Player* self = static_cast<MP3Player*>(_o);

    for(;;) {  
      while(!mCommandQueue.empty()) {
        auto command = mCommandQueue.pop();

        if (commandEvent.event == Event::CommandType::PLAY_MUSIC) {
          auto it = commandEvent.data.find(Event::SONGS_KEY);
          if(it != commandEvent.data.end()) {
            std::vector<int> songs = std::static_cast<std::vector<int>>(it->second);
            mSongQueue.clear();
            for(s: songs){mSongQueue.push(s);}
          }
                    
      setPlayingAction(PlayingAction::START);
    }
      }

      // if(self->getPlayingAction() == PlayingAction::START) {
      //   self->setPlayingAction(PlayingAction::NO_ACTION);
      //   Serial.println("We've been told to start");
      //   if(self->playNextSong()) {
      //     self->mCurrentTime = millis();
      //     self->setPlayingAction(PlayingAction::CONTINUE);
      //   }
      // }
      // else if(self->getPlayingAction() == PlayingAction::CONTINUE) {
      //   auto res = self->queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
      //   if(!res.startsWith(BT201Responses::RESPONSE_MUSIC_STATE_PLAYING)) {
      //     if(!self->playNextSong()) {
      //       self->setPlayingAction(PlayingAction::NO_ACTION);
      //     }                           
      //   }
      //   else {
      //     PlayStatus::SongStatus stat = self->updateStatus();
      //     if(millis() - self->mCurrentTime >= 5000) {
      //       Serial.println(stat.toString());
      //       self->mCurrentTime = millis();
      //     }
      //   }
      // }       
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
  
  void MP3Player::clearSerialBuffer() {
    while(BT201Serial.available()){BT201Serial.read();}
  }