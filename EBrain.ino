#include <vector>

#include "MP3Player.h"
#include "SmartPointer.h"
#include "DisplayManager.h"

#define MP3_RX D7
#define MP3_TX D6

#define DISPLAY_ONLY true
#define TEST_DISPLAY true

std::vector<ListItem> SONGVEC = {
ListItem(-1, DisplayManager::BACK, ""),
ListItem(-1, DisplayManager::ALL, ""),
ListItem(-1, DisplayManager::SHUFFLE, ""),
ListItem(1, "Song 1", "S1"),
ListItem(2, "Song 2", "S2"),
ListItem(3, "Song 3", "S3"),
ListItem(4, "Song 4", "S4")};

std::vector<ListItem> PLAYLISTS = {ListItem(1, "Title 1", "Dir1",{ListItem(-1, DisplayManager::BACK, "")}),
                              ListItem(2, "Title 2", "Dir2",{ListItem(-1, DisplayManager::BACK, "")}),
                              ListItem(3, "Title 3", "Dir3", SONGVEC),
                              ListItem(4, "Title 4", "Dir4",{ListItem(-1, DisplayManager::BACK, "")}),
                              ListItem(5, "Title 5", "Dir5",{ListItem(-1, DisplayManager::BACK, "")}),
                              ListItem(6, "Title 6", "Dir6",{ListItem(-1, DisplayManager::BACK, "")}),
                              ListItem(7, "Title 7", "Dir7",{ListItem(-1, DisplayManager::BACK, "")})};

const char* PLAYLIST_FILE = "/playlists.json";
shared_psram_ptr<DisplayManager> ptrDisplay = nullptr;
#ifndef DISPLAY_ONLY
unique_psram_ptr<MP3Control::MP3Player> ptrPlayer = nullptr;
#endif


void setup() {

  Serial.begin(115200);
  delay(3000);
  Serial.println("Starting....");
  if (psramInit()) {
    Serial.println("PSRAM initialized successfully!");
  } else {
    Serial.println("PSRAM not available or failed to initialize!");
    return;
  }

  #ifdef DISPLAY_ONLY
    Serial.println("DISPLAY_ONLY");
  #endif
  
  #ifndef DISPLAY_ONLY
  try {
    ptrPlayer = make_unique_psram<MP3Control::MP3Player>(MP3_RX, MP3_TX);
  } catch (std::bad_alloc e) {
    Serial.println("Could not make Mp3Player");
    return;
  }
  #endif

  try {
  #ifdef  TEST_DISPLAY
    ptrDisplay = make_shared_psram<DisplayManager>();
  #else
    ptrDisplay = make_shared_psram<DisplayManager>(PLAYLIST_FILE);
  #endif
  } catch (std::bad_alloc e) {
    Serial.println("Could not make Display");
    return;
  }
  
  Serial.println("All good");  
  Serial.printf("ptr Display: %d \n", ptrDisplay.get());

#ifndef DISPLAY_ONLY
  Serial.printf("ptr Player: %d \n", ptrPlayer.get());
#endif
 
#ifndef DISPLAY_ONLY
  if(ptrPlayer)
    ptrPlayer->initBT201();
#endif

 #ifdef  TEST_DISPLAY
  if(ptrDisplay) {
    ptrDisplay->setPlaylists(PLAYLISTS);
  }
  #endif

  Serial.println("All good 1 Done");
}

void loop() 
{
  if(Serial.available() > 0)
  {
    String command = Serial.readString();
    command.toUpperCase();
    Serial.println("Command: " + command);

    if(command == "S")
    {
      ptrDisplay->scroll(DisplayManager::PageDirection::DOWN);
    }
    else if(command == "W")
    {
      ptrDisplay->scroll(DisplayManager::PageDirection::UP);
    }
    else if(command == "X")
    {
      ptrDisplay->select();
    }
  }
  
}
