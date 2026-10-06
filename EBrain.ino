#include "MP3Player.h"
#include "SmartPointer.h"
#include "DisplayManager.h"

#define MP3_RX D7
#define MP3_TX D6



const char* PLAYLIST_FILE = "/playlists.json";
unique_psram_ptr<MP3Control::MP3Player> ptrPlayer = nullptr;
shared_psram_ptr<DisplayManager> ptrDisplay = nullptr;
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
  
  try {
    ptrPlayer = make_unique_psram<MP3Control::MP3Player>(MP3_RX, MP3_TX);
  } catch (std::bad_alloc e) {
    Serial.println("Could not make Mp3Player");
    return;
  }
  try {
    ptrDisplay = make_shared_psram<DisplayManager>(PLAYLIST_FILE);
  } catch (std::bad_alloc e) {
    Serial.println("Could not make Display");
    return;
  }
  
  Serial.println("All good");
  Serial.printf("ptr Player: %d \n", ptrPlayer.get());
  Serial.printf("ptr Display: %d \n", ptrDisplay.get());

  // if(ptrDisplay) {}

  // if(ptrPlayer)
  //   ptrPlayer->initBT201();
  Serial.println("All good 1 Done");
}

void loop() 
{
  
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
  /*
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
        ptrPlayer->queryBT201(command, !addend);
      }
      delay(2500);
    }
  }
  */
}
