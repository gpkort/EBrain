#include <stdexcept>
#include "DisplayManager.h"

DisplayManager::DisplayManager(const char* filePath) : mU8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE) {
  mU8g2.begin(); //u8g2_font_PixelTheatre_tr
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
  printToScreen();
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

void DisplayManager::printToScreen()
{
  mU8g2.clearDisplay();
  mU8g2.clearBuffer();					// clear the internal memory
  mU8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  mU8g2.drawStr(10,5,"PlayList");	// write something to the internal memory
  mU8g2.sendBuffer();					// transfer internal memory to the display
}

void DisplayManager::showPlaylists(const std::vector<String>& items)
{
  static constexpr int SEPARATOR_HEIGHT = 3;
  static constexpr int PADDING          = 2;   // gap around the separator

  mU8g2.clearBuffer();
  mU8g2.setFont(u8g2_font_6x10_tf);
  mU8g2.setFontPosTop();                       // y now refers to the top of the text

  const int lineHeight = mU8g2.getMaxCharHeight();
  const int width      = mU8g2.getDisplayWidth();
  const int height     = mU8g2.getDisplayHeight();

  // Title
  int y = 0;
  mU8g2.drawStr(0, y, "Playlists");
  y += lineHeight + PADDING;

  // 3 pixel separator
  mU8g2.drawBox(0, y, width, SEPARATOR_HEIGHT);
  y += SEPARATOR_HEIGHT + PADDING;

  // Vertical list
  for (const String& item : items)
  {
    if (y + lineHeight > height) break;        // stop when the next line would be clipped
    mU8g2.drawStr(0, y, item.c_str());
    y += lineHeight;
  }

  mU8g2.sendBuffer();
}



