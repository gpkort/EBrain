#pragma once

#include <stdexcept>
#include <map>
#include <vector>

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

struct SongList
{
  int index;
  String title;
  String fileName;
};
struct PlayList
{
  int index;
  String title;
  String dirName;
  std::map<int, SongList> songlist;
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

    DisplayManager(const char* filePath);
    ~DisplayManager()=default;

    void showPlaylists(const std::vector<String>& items);

  private:
    void parseJSON(JsonDocument& doc);
    void printToScreen();


    std::map<int, PlayList> mPlaylistsMap;
    U8G2_SH1106_128X64_NONAME_F_HW_I2C mU8g2;


};