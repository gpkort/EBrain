#include "MP3Player.h"

namespace MP3Control
{
   MP3Player::MP3Player(int8_t rx, int8_t tx)
    {
      BT201Serial.begin(BT201_BAUD_RATE, BT201_CONFIGURATION, rx, tx);
    }

    MP3Player::~MP3Player(){}

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

    //File Operations
    void MP3Player::readAllPlaylists()
    {
      //
    }

    void MP3Player::readPlaylist()
    {
      //
    }

    //Display Operations
    std::vector<String> MP3Player::getPlaylists()
    {
      std::vector<String> playlist;
      return playlist;
    }

    std::vector<String> MP3Player::getSongList()
    {
      std::vector<String> songs;
      return songs;
    }

} //MP3Control