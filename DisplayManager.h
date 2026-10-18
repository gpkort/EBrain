#pragma once

#include "class/hid/hid.h"
#include "WString.h"

#include <stdexcept>
#include <vector>
#include <optional>

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <ArduinoJson.h>
#include "LittleFS.h"
#include "Messages.h"

enum DisplayMode {
      INITIALIZING  = 0,
      PLAYLIST      = 1,
      SONGLIST      = 2,
      PLAYING       = 3,
      RECORDING     = 4
    };

struct ListItem
{
  int id = -1;
  int absoluteIndex = -1;
  String title = "";
  String pathName = "";
  std::vector<ListItem> songs;
};

class DisplayManager
{
  public:
    static constexpr char* PLAYLIST       = "playlists";
    static constexpr char* PLAYLIST_ID    = "id";
    static constexpr char* TITLE          = "name";
    
    static constexpr char* PLAYLIST_DIR   = "dir";
    static constexpr char* SONG_FILE      = "file";
    static constexpr char* SONG_LIST      = "songs";
    static constexpr char* SONG_INDEX     = "idx";

    static constexpr char* PLAYLISTS_HEADER  = "Playlists:";
    static constexpr char* SONGLIST_HEADER  = "Songs:";

    static constexpr int SEPARATOR_HEIGHT = 3;
    static constexpr int PADDING          = 2;   // gap around the separator

    static constexpr char* BACK       = "BACK";
    static constexpr char* ALL        = "ALL";
    static constexpr char* SHUFFLE    = "SHUFFLE";
    static constexpr int BACK_IDX     = 0;
    static constexpr int ALL_IDX      = 1;
    static constexpr int SHUFFLE_IDX  = 2;

    enum PageDirection{UP=0, DOWN=1};

    DisplayManager();
    DisplayManager(const char* filePath);
    ~DisplayManager()=default;
    
    void setPlaylists(std::vector<ListItem> pl, bool autoshow=true);
    void scroll(PageDirection direction);
    std::optional<SongPlay::PlayMessage> select();
    void resetDisplay();
        
  private:
    void parseJSON(JsonDocument& doc);
    int setDisplayHeader(char* headerText, int seperatorHeight = SEPARATOR_HEIGHT, int padding = PADDING);
    void updateScreenList(std::vector<ListItem> items, PageDirection direction, char* headerText);

    std::vector<ListItem> mPlaylists;
    std::vector<ListItem> mCurrentSonglist;
    U8G2_SH1106_128X64_NONAME_F_HW_I2C mU8g2;
    DisplayMode mDisplayMode = DisplayMode::INITIALIZING;
    int mSelectedIdx = 0;
    int mSelectedPlaylistIdx = -1;
    

};