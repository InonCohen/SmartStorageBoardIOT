//Written by Nick Koumaris
//info@educ8s.tv
#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PCF8574.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include "sntp.h"
#include "driver/i2c.h"
#include "SD.h"
#include "FS.h"
#include "ezButton.h"
// #include "FILE.h"

//define pins
#define RFID_RST_PIN 4
#define RFID_SS_PIN 5
#define GREEN_PIN 12
#define RED_PIN 13
#define BUZZER_PIN 27
#define SD_CS_PIN 15
#define OLED_SDA 21
#define SWITCH_PIN 26
#define BLUE_PIN 14
#define I2C_SDA_PIN 32
#define I2C_SCL_PIN 33


#define PCF8574_1_ADDRESS 0x20
#define PCF8574_2_ADDRESS 0x27
#define OLED_ADDRESS 0x3C

//define other CONSTANTS
#define TEXT_SIZE 1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define TOOLS_NUM 6
#define IN_BOARD 0

//Telegram BOT
#define BOTtoken "6214411428:AAF8yQOTw0OyQ_O9o7AAl6cjddUneeJ3pz0"  // your Bot Token (Get from Botfather)

//Telegram
#define CHAT_ID "1701605220"



//WifiFuncs
void NTPsetup();
char* GetTimeString();
//RFIDFuncs
void ReadTagID(String* uid);
bool CheckUID(String displayed_card_id, String* name_ptr, bool print_to_OLED = true);
//OLEDFuncs
void InitializeOLED();
// ExpanderToolFuncs
void InitializeTools();
void ToolLoop();
void UpdateTools();
void UpdateUserTools(String& user_name, String& uid);
void WriteTool(int tool_num, int led_state);
bool ReadTool(int tool_num);
void GetSwitchesState(String* arr);
//SDFuncs
void InitSDCard();
void clearSD();
//void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
bool readLine(File file, char* line, size_t maxLen);
void readFile(fs::FS &fs, const char * path, bool read_headers = false);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void deleteFile(fs::FS &fs, const char * path);
void ReadSDInitially();
void FillSDInitially();
void UpdateItem(int line_number, String uid = "");
void ReadItems(int arr[], const String& const_uid = "");
void UpdateLog(String& user_name);
void TurnOffBoard();
void TurnOnBoard();
void telegram_loop();
void OLED_setUP();


//declare global variables
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_SDA);
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN); // Instance of the class
ezButton door_switch(SWITCH_PIN);
Adafruit_PCF8574 pcf1;
Adafruit_PCF8574 pcf2;
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 10000;
unsigned long lastTimeBotRan;

const char* wifi_ssid       = "lib-test";
const char* wifi_password   = "test-3-8";


unsigned long startTime; // Variable to store the start time in milliseconds
const unsigned long duration = 120 * 1000; // seconds * 1000 = duration in milliseconds

enum CHANGE_IN_TOOLBOX {UNCHANGED, BORROWED, RETURNED};
enum LED_COLOR {BLUE, RED, GREEN};

int general_tools[TOOLS_NUM]; // 0 - the tool is borrowed, 1 - tool is present in the board
int current_user_tools[TOOLS_NUM]; // 0 - the tool is not the user's, 1 - tool is borrowed by the user
String tools_in_toolbox[TOOLS_NUM] = {"Pliers 1", "Cutter", "Pliers 2", "Screw 1", "Screw 2", "Screw 3"};
String switchesFirstState[TOOLS_NUM];
String switchesLastState[TOOLS_NUM];
CHANGE_IN_TOOLBOX tools_condition[TOOLS_NUM];
String tools_change_strings[3] = {"UNCHANGED", "BORROWED", "RETURNED"};

bool DoorChanged(){
  door_switch.loop(); // MUST call the loop() function first
  if (door_switch.isPressed()){
    // Serial.println("The door switch: ON -> OFF");
    // Serial.println("The door: OPEN -> CLOSED");
    Serial.println("The door switch: OPEN -> CLOSED");
    return true;
  }
  if (door_switch.isReleased()){
    // Serial.println("The door switch: OFF -> ON");
    // Serial.println("The door: CLOSED -> OPEN");
    Serial.println("The door switch: CLOSED -> OPEN");
    return true;
  }
  return false;
}

void SetLed(){
  pinMode(RED_PIN,   OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN,  OUTPUT);
}

void SetLedColor(LED_COLOR color) {
  int redValue = 255 * (color == RED);
  int greenValue = 255 * (color == GREEN);
  int blueValue = 255 * (color == BLUE);
  digitalWrite(RED_PIN, redValue);
  digitalWrite(GREEN_PIN, greenValue);
  digitalWrite(BLUE_PIN, blueValue);
}

void setup() 
{
  Serial.begin(115200);
  // delay(3000);
  SPI.begin(); // Init SPI bus
  
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);  // initialize with the I2C addr 0x3D (for the 128x64)
  SetLed();
  OLED_setUP();
  //i2c_init();
  // delay(3000);
  InitSDCard();
  // delay(3000);
  NTPsetup();
  delay(3000);
  FillSDInitially();
  delay(3000);
  ReadSDInitially();
  Wire1.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  // delay(3000);
  rfid.PCD_Init(); // Init MFRC522 
  door_switch.setDebounceTime(50);
  // delay(3000);
  InitializeTools();

  delay(100);
  SetLedColor(BLUE);

  // delay(1000);           
  Serial.println("Approximate your card to the reader...");
  // Clear the buffer.
  InitializeOLED();  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  TurnOffBoard();
  lastTimeBotRan = millis() - 5;
}

void loop()
{
  if (!rfid.PICC_IsNewCardPresent()) 
  {
    if (millis() > lastTimeBotRan + botRequestDelay)  
    {
      telegram_loop();
      delay(100);
      InitializeOLED();
    }
    return;
  }
  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial()) 
  {
    return;
  }
  String displayed_card_id = "";
  String user_name;
  ReadTagID(&displayed_card_id);
  //displayed_card_id.trim(); BAD!
  bool authorized = CheckUID(displayed_card_id, &user_name);
  if (!authorized)
  {
    SetLedColor(RED);
    Serial.println("Access denied");
    display.clearDisplay();
    display.setCursor(0,0); 
    display.print("Access denied");
    display.display();
    delay(2000);
    InitializeOLED();
    return;
  }
  SetLedColor(GREEN);
  int space_index = user_name.indexOf(' '); // Find the index of the first space
  String first_name = user_name.substring(0, space_index);
  while(!DoorChanged())
  {
    SetLedColor(GREEN);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Hello ");
    display.println(first_name);
    display.println("Please open the door");
    display.display();
  }
  startTime = millis();
  delay(100);
  // Serial.println("Going to update user_array");
  ReadItems(current_user_tools, displayed_card_id);
  Serial.println("CurrentUserTools: ");
  for(int i=0;i<TOOLS_NUM;i++){
    Serial.print(current_user_tools[i]);
  }
  Serial.print("\n");
    Serial.println("GeneralTools: ");
  for(int i=0;i<TOOLS_NUM;i++){
    Serial.print(general_tools[i]);
  }
  GetSwitchesState(switchesFirstState);
  Serial.println("Switches conditions first:");
  for (int j = 0; j < TOOLS_NUM; j++) {
    Serial.print(switchesFirstState[j]);
    Serial.print(" ");
  }
  Serial.print("\n");
  Serial.print("\n");
  while(!DoorChanged()){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Hello ");
    display.println(first_name);
    display.println("Door is open");
    display.display();
    int current_time = millis();
    if(current_time - startTime >= duration){
      digitalWrite(BUZZER_PIN, LOW);
      SetLedColor(RED);
     //add another 500 milliseconds of silence
    }
    ToolLoop();
  }
  digitalWrite(BUZZER_PIN, HIGH);
  SetLedColor(BLUE);
 //UPDATE USER'S TOOLS
  GetSwitchesState(switchesLastState);
  for (int j = 0; j < TOOLS_NUM; j++){
    if((current_user_tools[j] == 1 || general_tools[j] == 1) && (switchesFirstState[j] != switchesLastState[j])){
      if(switchesLastState[j] == "OFF"){
        tools_condition[j] = BORROWED;
      }
      else{
        tools_condition[j] = RETURNED;
      }
    }
  }
  delay(100);
  Serial.println("ToolsCondition: (0=unchanges, 1=borrowed, 2=returned");
  for (int j = 0; j < TOOLS_NUM; j++) {
    Serial.print(tools_condition[j]);
  }
  Serial.print("\n");
  UpdateUserTools(user_name, displayed_card_id);
  // Serial.print ("user tools: "); //Debug info
  // Serial.println(*current_user_tools);
  UpdateLog(user_name);
  readFile(SD,"/items.csv", true);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Door is closed");
  //digitalWrite(LED_PIN, LOW);  //change for actual led
  TurnOffBoard();
  display.print("Goodbye ");
  display.println(first_name);
  display.display();
  delay(2000);
  readFile(SD, "/log.txt", true);
  InitializeOLED();
  return;
}