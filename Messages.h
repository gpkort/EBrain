#pragma once

namespace SongPlay
{
  enum Action{
    UNSET       = 0,
    PLAY_SINGLE = 1,
    PLAY_ALL    = 2,
    SHUFFLE     = 3
  };

  struct PlayMessage {
    Action action = Action::UNSET;
    int startIdx = -1;
    int endIdx   = -1;   
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