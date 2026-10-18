#include <optional>
#include "HardwareSerial.h"
#include <iterator>
#include <stdexcept>
#include "DisplayManager.h"

DisplayManager::DisplayManager() : 
  mU8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE),
  mSelectedIdx(-1)
 {
  mU8g2.begin();
  mDisplayMode = DisplayMode::INITIALIZING;
};

DisplayManager::DisplayManager(const char* filePath) : DisplayManager() {
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
  mDisplayMode = DisplayMode::PLAYLIST;

  scroll((PageDirection::DOWN));
}

void DisplayManager::setPlaylists(std::vector<ListItem> pl, bool autoshow) {
  mPlaylists = std::move(pl);
  if(autoshow) {
    resetDisplay();
    mDisplayMode = DisplayMode::PLAYLIST;
    scroll(PageDirection::DOWN);
  }
}

void DisplayManager::scroll(PageDirection direction){
  if(mDisplayMode == DisplayMode::PLAYLIST) {
    updateScreenList(mPlaylists, direction, PLAYLISTS_HEADER);
  }
  else if(mDisplayMode == DisplayMode::SONGLIST) {
    updateScreenList(mCurrentSonglist, direction, SONGLIST_HEADER);
  }
}

std::optional<SongPlay::PlayMessage> DisplayManager::select() {  
  if(mSelectedIdx == -1) { return std::nullopt; }

  if(mDisplayMode == DisplayMode::PLAYLIST) {
    mSelectedPlaylistIdx = mSelectedIdx;
    mCurrentSonglist = mPlaylists[mSelectedPlaylistIdx].songs;
    mDisplayMode = DisplayMode::SONGLIST;
    resetDisplay();
    updateScreenList(mCurrentSonglist, PageDirection::DOWN, SONGLIST_HEADER);
  }
  else if(mDisplayMode == DisplayMode::SONGLIST) {
    int start = mCurrentSonglist[mSelectedIdx].absoluteIndex;
    int end =mCurrentSonglist.back().absoluteIndex;
    resetDisplay();

    switch (mSelectedIdx) {
      case BACK_IDX:        
        mSelectedIdx = mSelectedPlaylistIdx - 1;
        mSelectedPlaylistIdx = -1;
        updateScreenList(mPlaylists, PageDirection::DOWN, PLAYLISTS_HEADER);
        break;
      case ALL_IDX:
        return SongPlay::PlayMessage(SongPlay::Action::PLAY_ALL, start, end );
      case SHUFFLE_IDX:
        return SongPlay::PlayMessage(SongPlay::Action::SHUFFLE, start, end);
      default:          
        return SongPlay::PlayMessage(SongPlay::Action::PLAY_SINGLE, start);
    }
  }
  return std::nullopt;
}

void DisplayManager::resetDisplay() {
  mU8g2.clear();
  mSelectedIdx = -1;
}

void DisplayManager::parseJSON(JsonDocument& doc)
{
  int tidx = 1;
  mPlaylists.clear();
  JsonArray playlists = doc[PLAYLIST];

  for (JsonVariant pl : playlists) {    
    const char* plTitle = pl[TITLE];
    const char* plDir = pl[PLAYLIST_DIR];
    int plId = pl[PLAYLIST_ID];
    auto li = ListItem(plId, -1, plTitle, plDir);   
    li.songs = {ListItem(-1, -1, BACK, ""),
                ListItem(-1, -1, ALL, ""),
                ListItem(-1, -1, SHUFFLE, "")
                };

    JsonArray songs = pl[SONG_LIST];
    for(JsonVariant song : songs)
    {
      int idx = song[SONG_INDEX];
      const char* st = song[TITLE];
      const char* sf = song[SONG_FILE];
      li.songs.push_back(ListItem(idx, tidx, st, sf));
      tidx++;    
    }
    // for(auto& l:li) { Serial.println(l.pathName);}
    mPlaylists.push_back(li);
  }
}

void DisplayManager::updateScreenList(std::vector<ListItem> items, PageDirection direction, char* headerText)
{ 
  if(items.empty()) { return; }
  
  if(direction == PageDirection::UP)  {
    if(mSelectedIdx == 0) { return; }
    mSelectedIdx--;
  }
  else {
    if(mSelectedIdx == items.size() - 1) { return; }
    mSelectedIdx++;
  }
    
  int firstidx = mSelectedIdx == 0 ? 0 : mSelectedIdx - 1;

  mU8g2.clear();
  mU8g2.setFont(u8g2_font_6x10_tf);
  mU8g2.setFontPosTop();

  const int lineHeight = mU8g2.getMaxCharHeight();
  const int height     = mU8g2.getDisplayHeight();

  int y = setDisplayHeader(headerText);

  // Vertical list
  for(int i=firstidx; i<items.size();i++) {
    if (y + lineHeight > height) { break; }
    
    mU8g2.drawButtonUTF8(0, y, i == mSelectedIdx ? U8G2_BTN_INV : 0, 0, 2, 2, items[i].title.c_str());
    y += lineHeight + 2;
  }
 
  mU8g2.sendBuffer();
}

int DisplayManager::setDisplayHeader(char* headerText, int seperatorHeight, int padding)
{
  int y = 0;
  const int lineHeight = mU8g2.getMaxCharHeight();
  const int width      = mU8g2.getDisplayWidth();

  mU8g2.drawStr(0, y, headerText);
  y += lineHeight + padding;

  // 3 pixel separator
  mU8g2.drawBox(0, y, width, seperatorHeight);
  y += seperatorHeight + PADDING;

  return y;
}



