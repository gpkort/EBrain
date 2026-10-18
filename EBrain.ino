#include <vector>

#include "MP3Player.h"
#include "SmartPointer.h"
#include "DisplayManager.h"

#define MP3_RX D7
#define MP3_TX D6

/*
Display
-------
3.3 -> Vcc
Gnd->Gnd
gpio 5->SDA
gpio6->SCL

Bt201
VBUS->VCC
GND->GND
gpio44->TX
gpio43->RX
*/

const char* PLAYLIST_FILE = "/playlists.json";
shared_psram_ptr<DisplayManager> ptrDisplay = nullptr;
unique_psram_ptr<MP3Control::MP3Player> ptrPlayer = nullptr;

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


  try {
    ptrPlayer = make_unique_psram<MP3Control::MP3Player>(MP3_RX, MP3_TX);
  } catch (std::bad_alloc e) {
    Serial.println("Could not make Mp3Player");
    return;
  }

  // try {
  //   ptrDisplay = make_shared_psram<DisplayManager>(PLAYLIST_FILE);
  // } 
  // catch (std::bad_alloc e) {
  //   Serial.println("Could not make Display");
  //   return;
  // }
  
  Serial.printf("ptr Player: %d \n", ptrPlayer.get());
  //Serial.printf("ptr Display: %d \n", ptrDisplay.get()); 

  if(ptrPlayer)
    ptrPlayer->initBT201();

  Serial.println("All good 1 Done");
}

void loop() 
{
  if(Serial.available() > 0)
  {
    String command = Serial.readString();
    // if(!command.startsWith(":"))
    //   command.toUpperCase();

    Serial.println("Command: " + command);
    if(command == "S") {
      ptrPlayer->playAction(SongPlay::PlayMessage(SongPlay::Action::SHUFFLE, 17, 31));
    }
    else if(command == "A") {
      ptrPlayer->playAction(SongPlay::PlayMessage(SongPlay::Action::PLAY_ALL, 17));
    }
    else if(command == "PS") {
      ptrPlayer->playAction(SongPlay::PlayMessage(SongPlay::Action::PLAY_SINGLE, 18));
    }
    else if(command == "P") {
      ptrPlayer->play_pause();
    }
    else if(command.startsWith(":")) {
      Serial.println(ptrPlayer->queryBT201(command.substring(1).c_str()));
      
    }
    else {
      ptrPlayer->playSong(command.toInt());
    }
    // if(command == "S")
    // {
    //   ptrDisplay->scroll(DisplayManager::PageDirection::DOWN);
    // }
    // else if(command == "W")
    // {
    //   ptrDisplay->scroll(DisplayManager::PageDirection::UP);
    // }
    // else if(command == "X")
    // {
    //   auto action = ptrDisplay->select();
    //   if(action != std::nullopt) {
    //     Serial.println(action.value().toString());
    //     ptrDisplay = make_shared_psram<DisplayManager>(PLAYLIST_FILE);
    //   }
    //   //
    // }
  }
  
}
