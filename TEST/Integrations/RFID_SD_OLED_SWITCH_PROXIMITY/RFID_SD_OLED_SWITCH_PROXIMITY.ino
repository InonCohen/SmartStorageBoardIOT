//Written by Nick Koumaris
//info@educ8s.tv

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
#define CS_PIN_SD 15
#define OLED_SDA 21
#define SWITCH_PIN 33
#define TOOL_PIN 32
#define LED_PIN 2

#define TEXT_SIZE 1
#define TOOLS_NUM 1
#define IN_BOARD 0

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

Adafruit_SSD1306 display(OLED_SDA);

ezButton door_switch(SWITCH_PIN);
ezButton tool_switch(TOOL_PIN);

int general_tools[TOOLS_NUM];
int current_user_tools[TOOLS_NUM];

enum CHANGE_IN_TOOLBOX {UNCHANGED, BORROWED, RETURNED};
enum TOOLS_IN_TOOLBOX {SCREW};

CHANGE_IN_TOOLBOX tool_condition[TOOLS_NUM];

bool DoorChanged(){
  door_switch.loop(); // MUST call the loop() function first
  if (door_switch.isPressed()){
    Serial.println("The door switch: ON -> OFF");
    Serial.println("The door: OPEN -> CLOSED");
    return true;
  }
    if (door_switch.isReleased()){
    Serial.println("The door switch: OFF -> ON");
    Serial.println("The door: CLOSED -> OPEN");
    return true;
  }
  return false;
}

void ToolLoop(){
  Serial.println("In ToolLoop");
  tool_switch.loop(); // MUST call the loop() function first
  for (int i=0;i<TOOLS_NUM;i++){
    Serial.println("In for loop");
    Serial.print("i = ");
    Serial.println(i);
    Serial.println(current_user_tools[i]);
    if(current_user_tools[i]==1){
        if (tool_switch.isPressed()){
          Serial.println("The tool switch: ON -> OFF");
          Serial.println("The tool is RETURNED");
          tool_condition[i] = RETURNED;
          digitalWrite(LED_PIN, LOW);  
        }
        if (tool_switch.isReleased()){
          Serial.println("The tool switch: OFF -> ON");
          Serial.println("The tool is BORROWED");
          tool_condition[i] = BORROWED;
          digitalWrite(LED_PIN, HIGH);  
        }
        int state = tool_switch.getState();
        if (state == HIGH){
          // digitalWrite(LED_PIN, LOW); 
          Serial.println("The tool is HERE");
        }
        else{
          digitalWrite(LED_PIN, HIGH);  
          Serial.println("The tool is MISSING");
        }
        delay(500);
      }
  }
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


void ReadItems(int arr[], const String& const_uid = "") {
  // Initialize arr to all zeros
  Serial.println("In ReadItems");
  Serial.print("const_uid is ");
  Serial.println(const_uid);
  String uid = const_uid;
  uid.trim();
  Serial.print("uid == const_uid ? ");
  Serial.println(uid == const_uid);
  for (int i = 0; i < TOOLS_NUM; i++) {
    arr[i] = 0;
  }
  // Open the file for reading
  File file = SD.open("/items.csv");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  int lineNumber = 0; // Line number in the file
  String line;
  // Read each line from the file
  while (file.available()) {
    Serial.print("line ");
    Serial.print(lineNumber);
    Serial.println(" reads as follows: ");
    line = file.readStringUntil('\n');
    line.trim();
    Serial.println(line);
    lineNumber++;
    // Compare the line with uid
    if (lineNumber <= TOOLS_NUM) {
      Serial.print("uid == '' ? ");
      Serial.println(uid == "");
      // Set the corresponding element of arr to 1
      arr[lineNumber - 1] = (uid=="")? (line == IN_BOARD) : line.equals(uid);
    }
  }  
  // Close the file
  file.close();
}

void UpdateTools(){
  //update general_tools by the tools file
  //update all current_user to UNCHANGED
  for (int i = 0; i < TOOLS_NUM ;i++) {
    current_user_tools[i] = 0;
    tool_condition[i] = UNCHANGED;
    ReadItems(general_tools); //MUST CHANGE 
  }
}

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
      Serial.println("Failed to open file for reading");
      return;
  }
  Serial.println("Read from file:");
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

void ReadSDInitially(){
  readFile(SD, "/users.csv");
  readFile(SD, "/items.csv");
  readFile(SD, "/log.txt");
}

void FillSDinitially()
{
  listDir(SD, "/", 0);
  clearSD();
  listDir(SD, "/", 0);
  writeFile(SD, "/users.csv", "F9 CF C4 A3, Hila Levi, 0\n");
  appendFile(SD, "/users.csv", "59 C5 23 A4, Mais Fadila, 1\n");
  writeFile(SD, "/items.csv", "F9 CF C4 A3\n");
  writeFile(SD, "/log.txt", "initialize: \n");
  listDir(SD, "/", 0);
}

void InitializeOLED(){
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE); // or BLACK);
  display.setTextSize(TEXT_SIZE);
  display.setCursor(0,0); 
  display.println(" Welcome,\n Please pass tag");
  display.display();
  UpdateTools();
}

void ReadTagID(String* uid){
    //Show UID on serial monitor and on OLED
  Serial.print("UID tag :");
  byte letter;
  //fill displayed_card_id
  for (byte i = 0; i < rfid.uid.size; i++) 
  {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    (*uid).concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    (*uid).concat(String(rfid.uid.uidByte[i], HEX));
  }
  (*uid).toUpperCase();
}

bool CheckUID(String displayed_card_id, String* name_ptr){
  Serial.println();
  Serial.print("Message : ");
  displayed_card_id.toUpperCase();
  char* file_content = (char*)malloc(100);
  File file = SD.open("/users.csv");
  bool authorized = false;
  while(readLine(file, file_content, file.size())){
    char* token = strtok(file_content, ",");
    char *userID;
    if (token != NULL) {
    // Extract the content of the first item (column) into uid
      userID = token;
    }
    if (displayed_card_id.substring(1) == userID) //change here the UID of the card/cards that you want to give access
    {
        // Move to the next token (second column)
      token = strtok(NULL, ",");
      char* userName = token;
      String user_name_string(userName);
      user_name_string.trim();
      *name_ptr = user_name_string;
      Serial.print("user name is ");
      Serial.println(userName);
      Serial.println("Authorized access");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Hello");
      display.print(userName);
      display.display();
      authorized = true;
      Serial.println();
      delay(3000);
      break;
    }
  }
  return authorized;
}

void setup() {
  Serial.begin(115200);
  InitSDCard();
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  door_switch.setDebounceTime(50);
  tool_switch.setDebounceTime(50);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);           
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  FillSDinitially();
  ReadSDInitially();
  Serial.println("Approximate your card to the reader...");
  // Clear the buffer.
  InitializeOLED();  
}

void loop(){
  if (! rfid.PICC_IsNewCardPresent()) 
      {
        return;
      }
  // Select one of the cards
  if ( ! rfid.PICC_ReadCardSerial()) 
  {
    return;
  }
  String displayed_card_id = "";
  String user_name;
  ReadTagID(&displayed_card_id);
  //displayed_card_id.trim(); BAD!
  bool authorized = CheckUID(displayed_card_id, &user_name);
  if (!authorized){
    Serial.println("Access denied");
    display.clearDisplay();
    display.setCursor(0,0); 
    display.print("Access denied");
    display.display();
    delay(2500);
    InitializeOLED();
    return;
  }
  int space_index = user_name.indexOf(' '); // Find the index of the first space
  String first_name = user_name.substring(0, space_index);
  while(!DoorChanged()){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Hello ");
    display.println(first_name);
    display.println("Please open the door");
    display.display();
  }
  delay(100);
  Serial.println("Going to update user_array");
  ReadItems(current_user_tools, displayed_card_id);
  while(!DoorChanged()){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Hello ");
    display.println(first_name);
    display.println("Door is open");
    display.display();
    ToolLoop();
  }
  //Debug info
  Serial.print ("user tools: ");
  Serial.println(*current_user_tools);
  for (int i=0;i<TOOLS_NUM ;i++){
    if(!(tool_condition[i] == UNCHANGED || (tool_condition[i] == RETURNED && general_tools[i] == 1) || (tool_condition[i] == BORROWED && general_tools[i] == 0))) {
        char* log_txt = (char*)malloc(200);
        log_txt = "the tool is ";
        if(tool_condition[i] == RETURNED){
          strcat(log_txt, "returned by ");
        }
        else{
          strcat(log_txt, "borrowed by ");
        }
        strcat(log_txt, user_name.c_str());
        strcat(log_txt, " at \n");
        appendFile(SD, "/log.txt", log_txt);
        free(log_txt);
    }
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Door is closed");
  display.print("Good day ");
  display.println(first_name);
  display.display();
  delay(4000);
  InitializeOLED();
  return;
}