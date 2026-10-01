
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

String testValue = "";
bool readFile = false;

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

void loop() 
{
  if(ptrPlayer != nullptr)
  {
    ptrPlayer->responseHandler();
  }
  // if (BT201Serial.available() > 0) 
  // {
  //  if(readFile)
  //   {      
  //     String temp = "";
  //     while (BT201Serial.available() > 0) 
  //     {      
  //       char c = BT201Serial.read();
  //       if(c != ' ' && c != 'O' && c != 'K')
  //       {
  //         temp += c;
  //         if(temp.length() == 2)
  //         {
  //           int intc = (int)strtol(temp.c_str(), NULL, 16);
  //           char cc = (char)intc;
  //           testValue += cc;

  //           if(cc == '#')
  //           {
  //             Serial.println("found " + testValue);
  //             readFile = false;
  //             testValue = "";
  //             return;
  //           }
  //           temp = "";            
  //         }
  //       }
  //       delayMicroseconds(100);
  //     }
  //   }
  //   else 
  //   {
  //     Serial.println(BT201Serial.readString());    
  //   }
  // }
    
  if(Serial.available() > 0)
  {
    String command = Serial.readString();
    command.trim();
    Serial.println("Command: " + command);

    if(ptrPlayer != nullptr)
    {
      // bool addEnd = true;
      // readFile = false;
      // if(command.startsWith("AT+AR"))
      // {
      //   ptrPlayer->sendBT201CommandStr(command, true);
      // }
      if(command == "i")
      {
        ptrPlayer->initBT201();
      }
      else if(command.startsWith("AT+"))
      {
        bool addend = command.endsWith("#");

        if(addend)
        {
          command.replace("#", "");
        }
        Serial.println(ptrPlayer->queryBT201(command, !addend));
      }
      
    }
  }
}
