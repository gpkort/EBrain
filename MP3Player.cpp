#include "HardwareSerial.h"
#include "esp32-hal.h"
#include "WString.h"
#include "MP3Player.h"

namespace MP3Control
{
   MP3Player::MP3Player(int8_t rx, int8_t tx)
    {
      BT201Serial.setRxBufferSize(256);
      BT201Serial.begin(BT201_BAUD_RATE, BT201_CONFIGURATION, rx, tx);
      serialMutex = xSemaphoreCreateBinary();
      if(serialMutex != NULL)
      {
        xSemaphoreGive(serialMutex);
      }
    }

    MP3Player::~MP3Player(){}

    void MP3Player::responseHandler()
    {
      if (BT201Serial.available() > 0) 
      {
        if(1==1)
        {      
          String temp = "";
          mFileContent = "";
          mIsFileReady = false;
          while (BT201Serial.available() > 0) 
          {      
            char c = BT201Serial.read();
            if(c != ' ' && c != 'O' && c != 'K')
            {
              temp += c;
              if(temp.length() == 2)
              {
                int intc = (int)strtol(temp.c_str(), NULL, 16);
                char cc = (char)intc;
                mFileContent += cc;

                if(cc == '#')
                {
                  mIsFileReady = true;
                  return;
                }
                temp = "";            
              }
            }
            delayMicroseconds(100);
          }
        }
        else 
        {
          Serial.println(BT201Serial.readString());    
        }
      }
    }


    void MP3Player::sendBT201CommandStr(String cmd, bool sendEOL) {      
      if(sendEOL)
      {
        Serial.println("Adding");
        cmd += BT201Commands::ENDING;
      }
      Serial.println(cmd);
      BT201Serial.print(cmd.c_str());      
    }

    void MP3Player::sendBT201Command(const char* cmd) {
      Serial.println(cmd);
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

    void MP3Player::retrievePlaylistInfo()
    {
      if (xSemaphoreTake(serialMutex, 2000)) 
      {
        int try_num = 0;
        sendBT201Command(BT201Commands::READ_FILE);
        mFileContent = "";
        mIsFileReady = false;

        while(try_num < NUM_BUFFER_READS)
        {
          String temp = "";         
          if(BT201Serial.available() > 0) 
          {
            char c = BT201Serial.read();
            if(c != ' ' && c != 'O' && c != 'K')
            {
              temp += c;
              if(temp.length() == 2)
              {
                int intc = (int)strtol(temp.c_str(), NULL, 16);
                char cc = (char)intc;
                mFileContent += cc;

                if(cc == '#')
                {
                  try_num = NUM_BUFFER_READS;
                  break;
                }
                temp = "";
              }
            }
            delayMicroseconds(100);
          }
          delayMicroseconds(200);
        }
        xSemaphoreGive(serialMutex);
      }
    }

    void MP3Player::initBT201()
    {
      unsigned long start = millis();
      if(mMode == PlayerMode::NOTSET)
      {        
        auto res = queryBT201(BT201Commands::USE_SD_CARD);
        Serial.println("SD: " + res);

        res = queryBT201(BT201Commands::QUERY_MODE);  
        Serial.println("Mode: " + res);
        if(res == BT201Responses::SD_CARD_MUSIC_MODE)
        {
          Serial.println("GO MUSIC");
          mMode = PlayerMode::MUSIC;
        }
        res = queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
        Serial.println("PB: " + res);
        if(res == BT201Responses::RESPONSE_MUSIC_STATE_STOPPED) {
          mMusicState = MusicState::STOPPED;          
        }
        else if(res == BT201Responses::RESPONSE_MUSIC_STATE_PLAYING) {
          mMusicState = MusicState::PLAYING;
        }
        else if(res == BT201Responses::RESPONSE_MUSIC_STATE_PAUSED) {
          mMusicState = MusicState::PAUSED;
        }

        if(mMusicState != MusicState::STOPPED)
        {
          queryBT201(BT201Commands::QUERY_PLAYBACK_STATUS);
        }

      }
    }
    
    //Song Operations
    void MP3Player::play_pause()
    {
      sendBT201Command(BT201Commands::PP_SONG);
    }    

    void MP3Player::previousSong()
    {
      sendBT201Command(BT201Commands::PREVIOUS_SONG);
    }

    void MP3Player::nextSong()
    {
      sendBT201Command(BT201Commands::NEXT_SONG);
    }

    void MP3Player::playSong()
    {
      //
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
    
    std::vector<String> MP3Player::getPlaylist(String cmd)
    {
      String pl = "";
      std::vector<String> songs;
      // sendBT201Command(cmd.c_str()); 
      // unsigned long start = millis();
      // String message = "";
      // // while(true)
      // // {   
      //   if (BT201Serial.available() > 0) 
      //   {
      //     Serial.println("\n--- Data Received From BT201 ---");
          
      //     // Read the incoming bytes until the module finishes streaming
      //     while (BT201Serial.available() > 0) 
      //     {
      //       char c = BT201Serial.read();
      //       Serial.println(c); 
      //       message += String(c);
            
      //       // Tiny delay to allow serial buffer to fill up if data is streaming fast
      //       delayMicroseconds(100);
      //     }
      //   }
      //   Serial.println("\n--- End of Data Stream ---");
    
      // //   delay(750);
      // //   unsigned long passed = millis() - start;
      // //   if(passed % 500 == 0) Serial.println(passed);
      // //   if(passed > 10000) break;
      // // }
      // Serial.println(message);
      return songs;      
    }

    std::vector<String> MP3Player::getSongList()
    {
      // std::vector<String> playlist;
      // sendBT201Command(comm.c_str());
      // if (BT201Serial.available() > 0)
      // {
      //   auto message = BT201Serial.readString();
      //   Serial.println(message);  
      // }
      // return playlist;
    }

    String MP3Player::getResponse()
    {
      String serialString = "";
      Serial.println("Getting response " + (serialMutex != NULL ? String("ready") : String("NULL")));
      if (xSemaphoreTake(serialMutex, 2000)) 
      {
        Serial.println("Ready to read");
        int try_num = 0;
        while(try_num < NUM_BUFFER_READS)
        {
          if(BT201Serial.available() > 0)
          {
            serialString = BT201Serial.readString();
            break;
          }
          delayMicroseconds(1000);
          try_num++;
        }
        xSemaphoreGive(serialMutex);        
      }
      else {
        Serial.println("Can't Take");
      }
     
      return serialString;

    }

    
} //MP3Control