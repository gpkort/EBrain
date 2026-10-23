#include <vector>

#include "MP3Player.h"
#include "SmartPointer.h"
#include "DisplayManager.h"
#include "Messages.h"

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
TaskHandle_t inputTask = NULL;


void inputHandler(void* param) {
  std::function<void(Event::CommandEvent) func = std::static_cast<std::function<void(Event::CommandEvent)>(param);
  for(;;) {    
    if(Serial.available() > 0) {
      String command = Serial.readString();
      Serial.println("Command: " + command);

      Event::CommandEvent ce;
      if(command == "S") {
        std::vector<int> songs = {22, 17, 21, 12, 18 14};
        ce.event = Event::CommandType::PLAY_MUSIC;
        ce.data[Event::SONGS_KEY] = songs;
      }
      else if(command == "A") {
        std::vector<int> songs = {17, 18, 19, 20, 21 22};
        ce.event = Event::CommandType::PLAY_MUSIC;
        ce.data[Event::SONGS_KEY] = songs;
      }
      else if(command == "PS") {
        std::vector<int> songs = {18};
        ce.event = Event::CommandType::PLAY_MUSIC;
        ce.data[Event::SONGS_KEY] = songs;
      }
      else if(command == "PL") { ce.event = Event::CommandType::PLAY; }
      else if(command == "PA") { ce.event = Event::CommandType::PAUSE; }
      else if(command == "ST") { ce.event = Event::CommandType::STOP; }
      else if(command == "U") { ce.event = Event::CommandType::VOLUME_UP; }
      else if(command == "D") { ce.event = Event::CommandType::VOLUME_DOWN; }
      else if(command == "F") { ce.event = Event::CommandType::NEXT; }
      else if(command == "F") { ce.event = Event::CommandType::PREVIOUS; }

      if(ce.event != Event::CommandType::UNSET)
        func(ce);
      }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

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

  ptrPlayer = make_unique_psram<MP3Control::MP3Player>(MP3_RX, MP3_TX); 
  // ptrDisplay = make_shared_psram<DisplayManager>(PLAYLIST_FILE);  
  
  Serial.printf("ptr Player: %d \n", ptrPlayer.get());
  //Serial.printf("ptr Display: %d \n", ptrDisplay.get());

  if(ptrPlayer != nullptr && ptrPlayer->getMusicState == MP3Control::MP3Player::) {
    Serial.println("Ready Player One.}")
  } 

  xTaskCreatePinnedToCore(inputHandler, "input", 10000, ptrPlayer->playActionCB, 1, &inputTask, 0);
  Serial.println("All good 1 Done");
}

void loop() {}
