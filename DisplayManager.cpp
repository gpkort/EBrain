#include <stdexcept>
#include "DisplayManager.h"

DisplayManager::DisplayManager(const char* filePath) {
  if(!LittleFS.begin()){
    throw std::runtime_error("LittleFS not initialized");
  }
  File playlistsFile = LittleFS.open("/playlists.json");
  
  if (!playlistsFile) {
    throw std::runtime_error("Can't find playlist file.");
  }
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, playlistsFile);
  playlistsFile.close();
  if (error) {
    throw std::runtime_error(error.c_str());
  }
  
  parseJSON(doc);
}

void DisplayManager::parseJSON(JsonDocument& doc)
{
  mPlaylistsMap.clear();
  JsonArray playlists = doc[PLAYLIST];

  for (JsonVariant pl : playlists) {
    const char* plTitle = pl[TITLE];
    const char* plDir = pl[PLAYLIST_DIR];
    int plId = pl[PLAYLIST_ID];
    
    std::map<int, SongList> sl;
    JsonArray songs = pl[SONG_LIST];

    for(JsonVariant song : songs)
    {
      int idx = song[SONG_INDEX];
      const char* st = song[TITLE];
      const char* sf = song[SONG_FILE];
      sl[idx] = SongList(idx, st, sf);
    }
    mPlaylistsMap[plId] = PlayList(plId, plTitle, plDir, sl);
    Serial.println(plTitle);
  }
}



