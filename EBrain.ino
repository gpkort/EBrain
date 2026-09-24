
#include "MP3Player.h"

#define MP3_RX D7
#define MP3_TX D6


MP3Control::MP3Player* ptrPlayer = nullptr;
// TaskHandle_t  task1;

// void taskHandler(void * pvParameters) {
//   for(;;) {
//     if(ptrPlayer) {
//       ptrPlayer->responseHandler();
//     }
//     vTaskDelay(750 / portTICK_PERIOD_MS);
//   }
// }

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

  void* psramMemory = heap_caps_malloc(sizeof(MP3Control::MP3Player), MALLOC_CAP_SPIRAM);
  
  if (psramMemory == nullptr) {
    Serial.println("Failed to allocate memory in PSRAM!");
    return;
  }
  Serial.println("All good");
  
  ptrPlayer = new (psramMemory) MP3Control::MP3Player(MP3_RX, MP3_TX); 
  Serial.printf("ptr Board: %d \n", ptrPlayer);
  // xTaskCreatePinnedToCore(taskHandler, "SerialListener", 10000, NULL, 0, &task1, 0); 
  Serial.println("All good 1 Done");
}

void loop() {
    
  if(Serial.available() > 0)
  {
    String command = Serial.readString();
    command.trim();
    command.toUpperCase();

    if(ptrPlayer != nullptr)
    {
      if(command == "P")
      {
        ptrPlayer->play_pause();
      }
      else if(command == "N")
      {
        ptrPlayer->previousSong();
      }
      else if(command == "L")
      {
        ptrPlayer->nextSong();
      }
      else if(command == "I")
      {
        ptrPlayer->initBT201();
      }
      else
      {
        Serial.println("Unrecognized Command");
      }
    }
    
  }

}
