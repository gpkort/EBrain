#include "class/dfu/dfu.h"
#pragma once

#include <map>
#include <vector>
#include <HardwareSerial.h>
#include "Constants.h"
#include "Messages.h"

#define BT201Serial Serial1
typedef unsigned long ulong;


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
      NOT_SET      = 0,
      PLAYING_LIST = 1,
      PAUSED       = 2,
      STOPPED      = 3,
      IDLE         = 4
    };
          
    static const SerialConfig BT201_CONFIGURATION = SERIAL_8N1;
    static const unsigned long BT201_BAUD_RATE=115200;
    static constexpr char* TIMEOUT_MSG = "TIMEOUT";
    static constexpr int DEFAULT_TIMEOUT = 2000; 

    static constexpr int VOLUME_MIN = 0; 
    static constexpr int VOLUME_MAX = 30;

    static inline const String GENERIC_SUFFIX = "???";
    static void worker(void* _o);
    
    MP3Player(int8_t rx, int8_t tx);
    ~MP3Player();

    void sendBT201Command(const char* cmd);
    void sendBT201Command(String cmd, bool sendEOL=true);
    String queryBT201(const char* cmd);
    String queryBT201(String cmd, bool sendEOL=true);

    
    //Song Operations
    void playActionCB(Event::CommandEvent event);
    void play_pause();
    void initBT201();
    void previousSong();
    void nextSong();
    bool stopPlayer();

    //Volume
    void volumeUp();
    void volumeDown();
    void setVolume(int level);
    
    //Accessors
    int getVolume() { return mCurrentVolume; }
    void monitorPlaying();
    MusicState getMusicState(MusicState state);

  private:
    String getResponse(int timeout=DEFAULT_TIMEOUT);
    void clearSerialBuffer();
    bool setPlayerReady();
    void setMusicState(MusicState state);
    bool playNextSong();
    PlayStatus::SongStatus updateStatus();

    MusicState mMusicState = MusicState::NOT_SET;
    PlayerMode mMode = PlayerMode::NOTSET;
    PlayingAction mPlayingAction = PlayingAction::NO_ACTION;
    std::queue<int> mSongQueue;
    std::queue<Event::CommandEvent> mCommandQueue;

    int mCurrentVolume = -1;
    long mCurrentTime = -1;
    int mCurrentSongTotalTime = -1;
    
    SemaphoreHandle_t mSerialMutex = NULL;
    SemaphoreHandle_t mPlayingMutex = NULL;
    TaskHandle_t mWorkTask = NULL;
};

