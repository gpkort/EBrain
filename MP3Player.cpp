#include "MP3Player.h"

namespace MP3Control
{
   MP3Player::MP3Player(int8_t rx, int8_t tx)
    {
      BT201Serial.begin(BT201_BAUD_RATE, BT201_CONFIGURATION, rx, tx);
    }

    MP3Player::~MP3Player(){}

    /*
    M1+0000000E    The file playing by the current device is 0x0E
    M2+0000000F   The total number of files on the current device is 0x0F. Note that it is a legitimate audio file.
    MT+0000001E   The total time of the current file is 0x1E seconds
    MK+00000004    The current file has been played to 0x04 seconds
    MF+XXXXXXX    The name of the file currently playing is xxxxx.Here the file name is complete

    3.2.5 Music-related query instructions
    */

    void MP3Player::responseHandler()
    {
      if (BT201Serial.available() > 0)
      {
        auto message = BT201Serial.readString();
        Serial.println(message);        
      }
    }

    void MP3Player::sendBT201Command(const char* cmd) {
      Serial.println(cmd);
      BT201Serial.print(BT201Commands::AT_COMMAND);
      BT201Serial.print(cmd);
      BT201Serial.print(BT201Commands::ENDING);  
    }

    String MP3Player::queryBT201(const char* cmd, unsigned long timeout)
    {
      unsigned long start = millis();
      sendBT201Command(cmd);

      while((millis()-start) < timeout)
      {
        if (BT201Serial.available() > 0)
        {
          auto message = BT201Serial.readString();
          Serial.println(message);  
          return(message);     
        }
      }
      Serial.println(MP3Player::TIMEOUT_MSG);
      return(String(MP3Player::TIMEOUT_MSG));
    }

    //Song Operations
    void MP3Player::play_pause()
    {
      sendBT201Command(BT201Commands::PP_SONG);
    }
    void MP3Player::initBT201()
    {
      unsigned long start = millis();
      if(mMode == PlayerMode::NOT_SET)
      {
        sendBT201Command(BT201Commands::USE_SD_CARD);
        if(queryBT201(BT201Commands::QUERY_MODE) == String(BT201Responses::SD_CARD_MUSIC_MODE))
        {
          Serial.println("Good to go");
          mMode = PlayerMode::MUSIC;
        }
      }
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
    

    //File Operations
    // void MP3Player::readAllPlaylists()
    // {
    //   //
    // }

    // void MP3Player::readPlaylist()
    // {
    //   //
    // }

    //Display Operations
    std::vector<String> MP3Player::getPlaylist()
    {
      String pl = "";
      std::vector<String> songs;
      sendBT201Command("AT+AR/00_*.???\r\n"); 
      while(true)
      {   
        if (BT201Serial.available() > 0)
        {
          char c = BT201Serial.read();
          Serial.println(c);
          pl += String(c);

          if(c == "#")
          {
            Serial.println("Finished - " + pl);
            break;
          }

          if(pl.length() == 6 && pl.startsWith("ER+"))
          {
            Serial.println(pl);
            break;
          }
        }
      }
      return songs;      
    }

    std::vector<String> MP3Player::getSongList()
    {
      std::vector<String> playlist;
      sendBT201Command(comm.c_str());
      if (BT201Serial.available() > 0)
      {
        auto message = BT201Serial.readString();
        Serial.println(message);  
        return(message);     
      }
      return playlist;
    }

} //MP3Control