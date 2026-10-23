#include "class/msc/msc.h"
#pragma once

#include <vector>
#include <any>
/*
namespace SongPlay
{
  enum Action {
    UNSET         = 0,
    PLAY_MUSIC    = 1,
    PAUSE         = 2,
    PLAY          = 3;
    STOP          = 4;
    PREVIOUS      = 5;
    NEXT          = 6;    
  };
}//SongPlay

namespace PlayStatus
{
  struct SongStatus {
    int index = -1;
    int totalTime = -1;
    int playingTime = -1;
    
    String toString() {
      String ret = "{status:{\"index\": " + String(index) + ", ";
      ret += "\"totalTime\": " + String(totalTime) + ", ";
      ret += "\"playingTime\": " + String(playingTime) + ", ";
      return ret;
    }
  };
} //PlayStatus
*/

namespace Event {
  inline constexpr String SONGS_KEY = "songs";
  enum ChangeType {
    UNDEFINED      = 0;
    PLAYING        = 1;
    PAUSED         = 2;
    STOPPED        = 3;
    NEW_SONG       = 4;
    LIST_FINISHED  = 5;
    VOLUME         = 6;  
  }

 enum CommandType {
    UNSET              = 0,
    PLAY_MUSIC         = 1,
    PAUSE              = 2,
    PLAY               = 3;
    STOP               = 4;
    PREVIOUS_SONG      = 5;
    NEXT_SONG          = 6;
    VOLUME_UP          = 7;
    VOLUME_DOWN        = 8;
  };

  struct ChangeEvent {
    EventType event = EventType::UNDEFINED;
    std::map<String, std::any> data;
  }

  struct CommandEvent {
    CommandType event = EventType::UNSET;
    std::map<String, std::any> data;
  }
  
}