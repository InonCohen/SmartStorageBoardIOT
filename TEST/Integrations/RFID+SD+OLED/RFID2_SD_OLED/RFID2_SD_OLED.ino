// /*
//  * Connect the SD card to the following pins:
//  *
//  * SD Card | ESP32
//  *    D2       -
//  *    D3       SS
//  *    CMD      MOSI
//  *    VSS      GND
//  *    VDD      3.3V
//  *    CLK      SCK
//  *    VSS      GND
//  *    D0       MISO
//  *    D1       -
//  */


// #include "FS.h"
// #include "SD.h"
// #include "SPI.h"
// #include "MFRC522.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>


// #define SS_PIN_RC522 21 // Replace with your actual pin numbers
// #define RST_PIN_RC522 22
// #define CS_PIN_SD 5

// #define I2C_SDA 33
// #define I2C_SCL 32

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels

// TwoWire I2Coled = TwoWire(0);
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Coled, -1);

// MFRC522 mfrc522(SS_PIN_RC522, RST_PIN_RC522); // Create instance of MFRC522

// void clearSD() {
//   deleteFile(SD, "/users.csv");
//   deleteFile(SD, "/items.csv");
//   deleteFile(SD, "/log.txt");
// }

// void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
//     Serial.printf("Listing directory: %s\n", dirname);

//     File root = fs.open(dirname);
//     if(!root){
//         Serial.println("Failed to open directory");
//         return;
//     }
//     if(!root.isDirectory()){
//         Serial.println("Not a directory");
//         return;
//     }

//     File file = root.openNextFile();
//     while(file){
//         if(file.isDirectory()){
//             Serial.print("  DIR : ");
//             Serial.println(file.name());
//             if(levels){
//                 listDir(fs, file.path(), levels -1);
//             }
//         } else {
//             Serial.print("  FILE: ");
//             Serial.print(file.name());
//             Serial.print("  SIZE: ");
//             Serial.println(file.size());
//         }
//         file = root.openNextFile();
//     }
// }

// // void createDir(fs::FS &fs, const char * path){
// //     Serial.printf("Creating Dir: %s\n", path);
// //     if(fs.mkdir(path)){
// //         Serial.println("Dir created");
// //     } else {
// //         Serial.println("mkdir failed");
// //     }
// // }

// // void removeDir(fs::FS &fs, const char * path){
// //     Serial.printf("Removing Dir: %s\n", path);
// //     if(fs.rmdir(path)){
// //         Serial.println("Dir removed");
// //     } else {
// //         Serial.println("rmdir failed");
// //     }
// // }

// bool readLine(File file, char* line, size_t maxLen) {
//   for (size_t n = 0; n < maxLen; n++) {
//     int c = file.read();
//     if ( c < 0 && n == 0) return false;  // EOF
//     if (c < 0 || c == '\n') {
//       line[n] = 0;
//       return true;
//     }
//     line[n] = c;
//   }
//   return false; // line too long
// }

// void readFile(fs::FS &fs, const char * path){
//     Serial.printf("Reading file: %s\n", path);

//     File file = fs.open(path);
//     if(!file){
//         Serial.println("Failed to open file for reading");
//         return;
//     }

//     Serial.print("Read from file: ");
//     while(file.available()){
//         Serial.write(file.read());
//     }
//     file.close();
// }

// void writeFile(fs::FS &fs, const char * path, const char * message){
//     Serial.printf("Writing file: %s\n", path);

//     File file = fs.open(path, FILE_WRITE);
//     if(!file){
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     if(file.print(message)){
//         Serial.println("File written");
//     } else {
//         Serial.println("Write failed");
//     }
//     file.close();
// }

// void appendFile(fs::FS &fs, const char * path, const char * message){
//     Serial.printf("Appending to file: %s\n", path);

//     File file = fs.open(path, FILE_APPEND);
//     if(!file){
//         Serial.println("Failed to open file for appending");
//         return;
//     }
//     if(file.print(message)){
//         Serial.println("Message appended");
//     } else {
//         Serial.println("Append failed");
//     }
//     file.close();
// }

// // void renameFile(fs::FS &fs, const char * path1, const char * path2){
// //     Serial.printf("Renaming file %s to %s\n", path1, path2);
// //     if (fs.rename(path1, path2)) {
// //         Serial.println("File renamed");
// //     } else {
// //         Serial.println("Rename failed");
// //     }
// // }

// void deleteFile(fs::FS &fs, const char * path){
//     Serial.printf("Deleting file: %s\n", path);
//     if(fs.remove(path)){
//         Serial.println("File deleted");
//     } else {
//         Serial.println("Delete failed");
//     }
// }

// // void testFileIO(fs::FS &fs, const char * path){
// //     File file = fs.open(path);
// //     static uint8_t buf[512];
// //     size_t len = 0;
// //     uint32_t start = millis();
// //     uint32_t end = start;
// //     if(file){
// //         len = file.size();
// //         size_t flen = len;
// //         start = millis();
// //         while(len){
// //             size_t toRead = len;
// //             if(toRead > 512){
// //                 toRead = 512;
// //             }
// //             file.read(buf, toRead);
// //             len -= toRead;
// //         }
// //         end = millis() - start;
// //         Serial.printf("%u bytes read for %u ms\n", flen, end);
// //         file.close();
// //     } else {
// //         Serial.println("Failed to open file for reading");
// //     }


// //     file = fs.open(path, FILE_WRITE);
// //     if(!file){
// //         Serial.println("Failed to open file for writing");
// //         return;
// //     }

// //     size_t i;
// //     start = millis();
// //     for(i=0; i<2048; i++){
// //         file.write(buf, 512);
// //     }
// //     end = millis() - start;
// //     Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
// //     file.close();
// // }

// void initSD()
// {
//     listDir(SD, "/", 0);
//     clearSD();
//     listDir(SD, "/", 0);
//     writeFile(SD, "/users.csv", "");
//     writeFile(SD, "/items.csv", "");
//     appendFile(SD, "/items.csv", "item1, Locker\n");
//     writeFile(SD, "/log.txt", "initialize: \n");
//     listDir(SD, "/", 0);
// }


// void setup(){
//     Serial.begin(115200);
//     SPI.begin();
//     if(!SD.begin(CS_PIN_SD)){
//         Serial.println("Card Mount Failed");
//         return;
//     }
//     bool oled_status;
//     I2Coled.begin(I2C_SDA, I2C_SCL, 400000);
//     oled_status = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//     if(!oled_status) { // Address 0x3D for 128x64
//       Serial.println("Oled Initialization failed");
//       for(;;);
//     }
//     delay(2000);
//     display.clearDisplay();
//     InitOled();
//     mfrc522.PCD_Init();
//     uint8_t cardType = SD.cardType();

//     if(cardType == CARD_NONE){
//         Serial.println("No SD card attached");
//         return;
//     }

//     Serial.print("SD Card Type: ");
//     if(cardType == CARD_MMC){
//         Serial.println("MMC");
//     } else if(cardType == CARD_SD){
//         Serial.println("SDSC");
//     } else if(cardType == CARD_SDHC){
//         Serial.println("SDHC");
//     } else {
//         Serial.println("UNKNOWN");
//     }

//     uint64_t cardSize = SD.cardSize() / (1024 * 1024);
//     Serial.printf("SD Card Size: %lluMB\n", cardSize);


//     // createDir(SD, "/mydir");
//     // listDir(SD, "/", 0);
//     // removeDir(SD, "/mydir");
//     // listDir(SD, "/", 2);
//     initSD();
  
//     appendFile(SD, "/users.csv", "B7 41 65 62, Hila Levi, 0\n");
//     appendFile(SD, "/users.csv", "59 C5 23 A4, Mais Fadila, 1\n");
//     listDir(SD, "/", 0);
//     Serial.println("Approximate your card to the reader...");

    
//     // appendFile(SD, "/hello.txt", "World!\n");
//     // readFile(SD, "/hello.txt");
//     // deleteFile(SD, "/foo.txt");
//     // renameFile(SD, "/hello.txt", "/foo.txt");
//     // readFile(SD, "/foo.txt");
//     // testFileIO(SD, "/test.txt");
//     // Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
//     // Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
// }

// void loop(){
//   if ( ! mfrc522.PICC_IsNewCardPresent()) 
//   {
//     return;
//   }
//   // Select one of the cards
//   if ( ! mfrc522.PICC_ReadCardSerial()) 
//   {
//     return;
//   }
//   //Show UID on serial monitor
//   Serial.print("UID tag :");
//   String content= "";
//   byte letter;
//   for (byte i = 0; i < mfrc522.uid.size; i++) 
//   {
//      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//      Serial.print(mfrc522.uid.uidByte[i], HEX);
//      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
//      content.concat(String(mfrc522.uid.uidByte[i], HEX));
//   }
//   Serial.println();
//   Serial.print("Message : ");
//   content.toUpperCase();
//   char* file_content = (char*)malloc(100);
//   File file = SD.open("/users.csv");
//   bool authorized = false;
//   while(readLine(file, file_content, file.size())){
//     char* userID = strtok(file_content, ",");
//     if (content.substring(1) == userID) //change here the UID of the card/cards that you want to give access
//     {
//       Serial.println("Authorized access");
//       authorized = true;
//       Serial.println();
//       delay(3000);
//       break;
//     }
//   }
//   if (!authorized)  {
//     Serial.println("Access denied");
//     delay(3000);
//   }
// }


// //###############################Functions###############################
// // void listDir(fs::FS &fs, const char * dirname, uint8_t levels) - Exist
// // void createDir(fs::FS &fs, const char * path)
// // void removeDir(fs::FS &fs, const char * path)
// // void readFile(fs::FS &fs, const char * path) - Exist
// // void writeFile(fs::FS &fs, const char * path, const char * message) - Exist
// // void appendFile(fs::FS &fs, const char * path, const char * message) - Exist
// // void renameFile(fs::FS &fs, const char * path1, const char * path2)
// // void deleteFile(fs::FS &fs, const char * path) - Exist
// // void testFileIO(fs::FS &fs, const char * path)





// /*********
//   Rui Santos
//   Complete project details at https://randomnerdtutorials.com  
// *********/
// void InitOled(){
//   delay(2000);
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 10);
//   // Display static text
//   display.println("Please pass your tag");
//   display.display(); 
// }




//Written by Nick Koumaris
//info@educ8s.tv

#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FS.h"
#include "SD.h"

#define SS_PIN 5
#define RST_PIN 4
#define CS_PIN_SD 15
#define OLED_SDA 21

#define TEXT_SIZE 1
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
Adafruit_SSD1306 display(OLED_SDA);
MFRC522::MIFARE_Key key; 

int code[] = {0xB7,0x41,0x65,0x62}; //This is the stored UID
int codeRead = 0;
String uidString;

void InitSDCard(){
  if(!SD.begin(CS_PIN_SD)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      return;
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }
}

void clearSD() {
  deleteFile(SD, "/users.csv");
  deleteFile(SD, "/items.csv");
  deleteFile(SD, "/log.txt");
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

bool readLine(File file, char* line, size_t maxLen) {
  for (size_t n = 0; n < maxLen; n++) {
    int c = file.read();
    if ( c < 0 && n == 0) return false;  // EOF
    if (c < 0 || c == '\n') {
      line[n] = 0;
      return true;
    }
    line[n] = c;
  }
  return false; // line too long
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void initSD()
{
    listDir(SD, "/", 0);
    clearSD();
    listDir(SD, "/", 0);
    writeFile(SD, "/users.csv", "");
    writeFile(SD, "/items.csv", "");
    appendFile(SD, "/items.csv", "item1, Locker\n");
    writeFile(SD, "/log.txt", "initialize: \n");
    listDir(SD, "/", 0);
}

void setup() {
  
  Serial.begin(115200);
  InitSDCard();
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  initSD();
  appendFile(SD, "/users.csv", "B7 41 65 62, Hila Levi, 0\n");
  appendFile(SD, "/users.csv", "59 C5 23 A4, Mais Fadila, 1\n");
  listDir(SD, "/", 0);
  Serial.println("Approximate your card to the reader...");
  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE); // or BLACK);
  display.setTextSize(TEXT_SIZE);
  display.setCursor(10,0); 
  display.print("Welcome, Pass tag");
  display.display();
  
}

void loop(){
  if ( ! rfid.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! rfid.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < rfid.uid.size; i++) 
  {
     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(rfid.uid.uidByte[i], HEX);
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  char* file_content = (char*)malloc(100);
  File file = SD.open("/users.csv");
  bool authorized = false;
  while(readLine(file, file_content, file.size())){
    char* userID = strtok(file_content, ",");
    if (content.substring(1) == userID) //change here the UID of the card/cards that you want to give access
    {
      Serial.println("Authorized access");
      authorized = true;
      Serial.println();
      delay(3000);
      break;
    }
  }
  if (!authorized)  {
    Serial.println(" Access denied");
    delay(3000);
  }
}

void readRFID()
{
  
  rfid.PICC_ReadCardSerial();
  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

    clearUID();
   
    Serial.println("Scanned PICC's UID:");
    printDec(rfid.uid.uidByte, rfid.uid.size);

    uidString = String(rfid.uid.uidByte[0])+" "+String(rfid.uid.uidByte[1])+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);
    
    printUID();

    int i = 0;
    boolean match = true;
    while(i<rfid.uid.size)
    {
      if(!(rfid.uid.uidByte[i] == code[i]))
      {
           match = false;
      }
      i++;
    }

    if(match)
    {
      Serial.println("\nI know this card!");
      printUnlockMessage();
    }else
    {
      Serial.println("\nUnknown Card");
    }


    // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

  void clearUID()
  {
    display.setTextColor(BLACK); // or BLACK);
    display.setTextSize(TEXT_SIZE);
    display.setCursor(30,20); 
    display.print(uidString);
    display.display();
  }

  void printUID()
  {
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(TEXT_SIZE);
    display.setCursor(0,20); 
    display.print("UID: ");
    display.setCursor(30,20); 
    display.print(uidString);
    display.display();
  }

  void printUnlockMessage()
  {    
    display.setTextColor(WHITE); // or BLACK);
    display.setTextSize(TEXT_SIZE);
    display.setCursor(10,0); 
    display.print("Unlocked");
    display.display();
    
    delay(3000);
    


  }