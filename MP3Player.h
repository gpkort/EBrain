#pragma once

#include <vector>

#include <HardwareSerial.h>
#include "Constants.h"

#define BT201Serial Serial1

namespace MP3Control
{
  class MP3Player
  {
    public:
      enum PlayerMode
      {
        NOT_SET = 0,
        MUSIC = 1,
        RECORD = 2
      };
      
      enum PlayerSource
      {
        UKNOWN = 0,
        SD_CARD = 1
        // USB_DRIVE = 2,
        // BLUTOOTH = 3
      };

      static const SerialConfig BT201_CONFIGURATION = SERIAL_8N1;
      static const unsigned long BT201_BAUD_RATE=115200;
      static constexpr char* TIMEOUT_MSG = "TIMEOUT"; 

      static constexpr int VOLUME_MIN = 0; 
      static constexpr int VOLUME_MAX = 30; 
    
      MP3Player(int8_t rx, int8_t tx);
      ~MP3Player();

      void responseHandler();
      void sendBT201Command(const char* cmd);
      String queryBT201(const char* cmd, unsigned long timeout=2000);

      //Query


      //Song Operations
      void play_pause();
      void initBT201();
      void previousSong();
      void nextSong();
      void playSong();

      //File Operations
      void readAllPlaylists();
      void readPlaylist();

      //Volume
      void volumeUp();
      void volumeDown();
      void setVolume(int level);
      

      //Display Operations
      std::vector<String> getPlaylist();
      std::vector<String> getSongList();

      //Accessors
      int getVolume() { return mCurrentVolume; }

    private:
      bool mIsPlaying;
      PlayerMode mMode = PlayerMode::NOT_SET;
      PlayerSource mSource = PlayerSource::UKNOWN;
      int mCurrentVolume = -1;

  };
} //MP3Control

