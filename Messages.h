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
    String path = "";    
  };
}//SongPlay