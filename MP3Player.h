#pragma once

#include <map>
#include <vector>
#include <HardwareSerial.h>
#include "Constants.h"

#define BT201Serial Serial1
typedef unsigned long ulong;

namespace MP3Control
{
  class MP3Player
  {
    public:
      enum PlayerMode
      {
        NOTSET = 0,
        MUSIC = 1,
        RECORD = 2
      };

      enum MusicState
      {
        NOT_SET = 0,
        PLAYING = 1,
        PAUSED  = 2,
        STOPPED = 3
      };
      
      static const SerialConfig BT201_CONFIGURATION = SERIAL_8N1;
      static const unsigned long BT201_BAUD_RATE=115200;
      static constexpr char* TIMEOUT_MSG = "TIMEOUT";
      static constexpr int DEFAULT_TIMEOUT = 5000; 

      static constexpr int VOLUME_MIN = 0; 
      static constexpr int VOLUME_MAX = 30;

      static constexpr char PLAYLIST_SEPERATOR = '|';
      
      MP3Player(int8_t rx, int8_t tx);
      ~MP3Player();

      void sendBT201Command(const char* cmd);
      void sendBT201CommandStr(String cmd, bool sendEOL=true);
      String queryBT201(const char* cmd);
      String queryBT201(String cmd, bool sendEOL=true);

      //Display 
      void setPlaylists();


      //Song Operations
      void play_pause();
      void initBT201();
      void previousSong();
      void nextSong();
      void playSong();

      bool setPlayerMode();
      bool stopPlayer();

      //File Operations
      void readAllPlaylists();
      void readPlaylist();

      //Volume
      void volumeUp();
      void volumeDown();
      void setVolume(int level);
      

      //Display Operations
      std::vector<String> getPlaylist(String cmd);
      std::vector<String> getSongList();

      //Accessors
      int getVolume() { return mCurrentVolume; }
    private:
      String getResponse(int timeout=DEFAULT_TIMEOUT);
      void clearSerialBuffer();

      MusicState mMusicState = MusicState::NOT_SET;
      PlayerMode mMode = PlayerMode::NOTSET;

      int mCurrentVolume = -1;
      
      SemaphoreHandle_t serialMutex = NULL;
  };
} //MP3Control

