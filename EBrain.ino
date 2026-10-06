
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

String fileContents = "01_Welcome_Interstate_Managers,16_Yours_And_Mine,15_Supercollider,17_Elevator_Up,05_No_Better_Place,07_All_Kinds_Of_Time,11_Hung_Up_On_You,14_Bought_For_A_Song,06_Valley_Winter_Song,08_Little_Red_Light,10_Halleys_Waitress,13_Peace_And_Love,03_Stacys_mom,04_Hackensack,09_Hey_ales|02_The_Flyer|02_The_Flyer,01_The_FlSouthbound_Train,05_These_Days_in_an_Open_Book,06_Time_of_Inconvenience,07 Dont_Forget_About_Me,08_Always_Will,09_Going_Back_to_Georgia,10_Talk_to_Me_While_Im_Listening,11_Fragile,12_On_Grafton_Street,13_Anything_You_Need_but_Me,14_Goodnight_to_Mothers_Dream,15_This_Heart#";
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
  // if(ptrPlayer)
  //   ptrPlayer->initBT201();

  // if(ptrPlayer) ptrPlayer->parsePlaylist(fileContents);
  
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
