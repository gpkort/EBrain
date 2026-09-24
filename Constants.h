#pragma once

namespace MP3Control
{
  class BT201Commands
  {
    public:
      static constexpr char* AT_COMMAND  = "AT+";
      static constexpr char* ENDING      = "\r\n";

      // MODES
      static constexpr char* USE_BT        = "CM01";
      static constexpr char* USE_USB_DRIVE = "CM02";
      static constexpr char* USE_SD_CARD   = "CM03";

      // MUSIC
      static constexpr char* PP_SONG        = "CB";
      static constexpr char* NEXT_SONG      = "CC";
      static constexpr char* PREVIOUS_SONG  = "CE";

      //VOLUME
      static constexpr char* RAISE_VOLUME   = "CB";
      static constexpr char* LOWER_VOLUME   = "CC";
      static constexpr char* TOGGLE_MUTE    = "CU02";
      static constexpr char* SET_VOLUME     = "QA";

      //Query   
      static constexpr char* QUERY_MODE                   = "QM";
      static constexpr char* QUERY_VOLUME                 = "QA";
      static constexpr char* QUERY_FILE_NUM               = "M1";
      static constexpr char* QUERY_PLAYBACK_MODE          = "MC";
      static constexpr char* QUERY_PLAYBACK_DEVICE        = "MD";
      static constexpr char* QUERY_PLAYBACK_STATUS        = "MP";
      static constexpr char* QUERY_PLAYBACK_TOTAL_TIME    = "MT";
      static constexpr char* QUERY_PLAYBACK_PLAYING_TIME  = "MK";

      //  MK, MT
  };

  class BT201Responses
  {
    public:
      static constexpr char* SD_CARD_MUSIC_MODE              = "QM+03";
      static constexpr char* RESPONSE_FILE_NUM               = "M1";
      static constexpr char* RESPONSE_PLAYBACK_MODE          = "MC";
      static constexpr char* RESPONSE_PLAYBACK_DEVICE        = "MD";
      static constexpr char* RESPONSE_PLAYBACK_STATUS        = "MP";
      static constexpr char* RESPONSE_PLAYBACK_TOTAL_TIME    = "MT";
      static constexpr char* RESPONSE_PLAYBACK_PLAYING_TIME  = "MK";
  };
}//BT201