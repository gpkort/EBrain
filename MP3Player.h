#pragma once

#include <map>
#include <vector>
#include <HardwareSerial.h>
#include "Constants.h"
#include "Messages.h"

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
      static constexpr int DEFAULT_TIMEOUT = 1000; 

      static constexpr int VOLUME_MIN = 0; 
      static constexpr int VOLUME_MAX = 30;

      static inline const String MP3_SUFFIX = ".mp3";
      static inline const String GENERIC_SUFFIX = "???";
      
      MP3Player(int8_t rx, int8_t tx);
      ~MP3Player();

      void sendBT201Command(const char* cmd);
      void sendBT201CommandStr(String cmd, bool sendEOL=true);
      String queryBT201(const char* cmd);
      String queryBT201(String cmd, bool sendEOL=true);

      
      //Song Operations
      void playAction(SongPlay::PlayMessage message);
      void play_pause();
      void initBT201();
      void previousSong();
      void nextSong();
      void playSong(int index);            
      bool stopPlayer();

      //Volume
      void volumeUp();
      void volumeDown();
      void setVolume(int level);
      
      //Accessors
      int getVolume() { return mCurrentVolume; }
      void monitorPlaying();

      SemaphoreHandle_t getPlayingMutex() {return mPlayingMutex;}
    private:
      
      static void taskWrapper(void* _o);
      static SemaphoreHandle_t mPlayingMutex = NULL;

      String getResponse(int timeout=DEFAULT_TIMEOUT);
      void clearSerialBuffer();
      bool setPlayerMode();
      void setSongPlayingMode();
      void playSongs(std::vector<int>& songs);
      

      MusicState mMusicState = MusicState::NOT_SET;
      PlayerMode mMode = PlayerMode::NOTSET;
      std::map<int, bool> mSongMap;

      int mCurrentVolume = -1;
      
      SemaphoreHandle_t mSerialMutex = NULL;
      
      TaskHandle_t mMonitorTask = NULL;
  };
} //MP3Control

