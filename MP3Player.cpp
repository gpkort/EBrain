#include "WString.h"
#include "MP3Player.h"

namespace MP3Control
{
   MP3Player::MP3Player(int8_t rx, int8_t tx)
    {
      BT201Serial.setRxBufferSize(256);
      BT201Serial.begin(BT201_BAUD_RATE, BT201_CONFIGURATION, rx, tx);
      serialMutex = xSemaphoreCreateBinary();
      if(serialMutex != NULL) { xSemaphoreGive(serialMutex); }
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

    String MP3Player::queryBT201(String cmd, bool sendEOL)
    {
      sendBT201CommandStr(cmd, sendEOL);
      return getResponse();
    }

    String MP3Player::queryBT201(const char* cmd)
    {
      sendBT201Command(cmd);
      return getResponse();
    }

    bool MP3Player::setPlayerMode()
    {
      queryBT201(BT201Commands::USE_SD_CARD);
      delayMicroseconds(200);
      auto res = queryBT201(BT201Commands::QUERY_MODE);
      res.trim();
      Serial.println("Res: " + res);   
      return res.startsWith(BT201Responses::SD_CARD_MUSIC_MODE);
    }

    bool MP3Player::stopPlayer()
    {
      queryBT201(BT201Commands::STOP_SONG);
      delay(200);
      auto res = queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
      return  res.startsWith(BT201Responses::RESPONSE_MUSIC_STATE_STOPPED);
    }

    void MP3Player::initBT201()
    {
      unsigned long start = millis();
      
      if(mMode != PlayerMode::MUSIC)
      {        
        if(setPlayerMode())
        {
          mMode == PlayerMode::MUSIC;          
        }
        else {
          Serial.println("Can't set mode");
          return;
        }
        if(stopPlayer())
        {
          Serial.println("Setting state");
          mMusicState = MusicState::STOPPED;
        }
        else {
          Serial.println("Can't set state");
          return;
        }
      }              
    }
        
    //Song Operations
    void MP3Player::playAction(SongPlay::PlayMessage message) {
      switch (message.action) {
        case SongPlay::Action::PLAY_ALL:
          Serial.println("Play All: " + message.path);
          // TODO: implement
          return;
        case SongPlay::Action::PLAY_SINGLE:
          Serial.println("Play Song: " + message.path);
          playSong(message.path);
          return;
        case SongPlay::Action::SHUFFLE:
          Serial.println("Shuffle: " + message.path);
          // TODO: implement
          return;
        case SongPlay::Action::UNSET:
        default:
          Serial.print("No Action");     
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

    void MP3Player::playSong(String file)
    {
      Serial.println(BT201Commands::PLAY_SPECIFIC_SONG + file);
      auto res = queryBT201(BT201Commands::PLAY_SPECIFIC_SONG + file);//, false);
      
      Serial.println("Song res= " + res);
    }

    void MP3Player::volumeUp()
    {
      sendBT201Command(BT201Commands::RAISE_VOLUME);
      ++mCurrentVolume;
    }

    void MP3Player::volumeDown()
    {
      sendBT201Command(BT201Commands::LOWER_VOLUME);
      --mCurrentVolume;
    }
    
    void MP3Player::setVolume(int level)
    {
      if(level >= VOLUME_MIN and level <= VOLUME_MAX)
      {
        String comm = String(BT201Commands::SET_VOLUME) + String(level);
        sendBT201Command(comm.c_str());
        mCurrentVolume = level;
      }
    }
            
    String MP3Player::getResponse(int timeout)
    {
      String serialString = "";
      int start = millis();
      if (xSemaphoreTake(serialMutex, 2000)) 
      {
        int try_num = 0;
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
        xSemaphoreGive(serialMutex);        
      }
           
      return serialString;
    }

    void MP3Player::clearSerialBuffer()
    {
      while(BT201Serial.available()){BT201Serial.read();}
    }

} //MP3Control