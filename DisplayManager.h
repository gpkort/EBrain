#include <stdexcept>
#include <map>

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
    ~DisplayManager(){};

  private:
    void parseJSON(JsonDocument& doc);
    std::map<int, PlayList> mPlaylistsMap;




};