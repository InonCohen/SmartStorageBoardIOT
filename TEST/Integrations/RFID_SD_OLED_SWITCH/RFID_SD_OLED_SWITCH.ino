#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ezButton.h>
#include "FS.h"
#include "SD.h"

#define SS_PIN 5
#define RST_PIN 4
#define CS_PIN_SD 33
#define OLED_SDA 21
#define SWITCH_PIN 32

#define TEXT_SIZE 1
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
Adafruit_SSD1306 display(OLED_SDA);
MFRC522::MIFARE_Key key; 
ezButton mySwitch(SWITCH_PIN);

String uidString;

void SwitchLoop(){
    mySwitch.loop(); // MUST call the loop() function first

  if (mySwitch.isPressed())
    Serial.println("The switch: OFF -> ON");

  if (mySwitch.isReleased())
    Serial.println("The switch: ON -> OFF");

  int state = mySwitch.getState();
  if (state != HIGH)
    Serial.println("The switch: OFF");
  else
    Serial.println("The switch: ON");
}


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
  SwitchLoop();
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