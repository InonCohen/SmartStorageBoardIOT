//Written by Nick Koumaris
//info@educ8s.tv
#include <Arduino.h>
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
#include "time.h"
#include "sntp.h"
#include "driver/i2c.h"
#include "SD.h"
#include "FS.h"
#include "ezButton.h"

//define pins
#define SWITCH_PIN 33
#define TOOL_PIN 32
#define LED_PIN 2
#define BUZZER_PIN 14
#define I2C_SDA_PIN 26
#define I2C_SCL_PIN 27
#define OLED_SDA 21
#define RFID_SS_PIN 5
#define RFID_RST_PIN 4
#define SD_CS_PIN 15
#define PCF8574_1_ADDRESS 0x20
#define PCF8574_2_ADDRESS 0x27
#define OLED_ADDRESS 0x3C

//define other CONSTANTS
#define TEXT_SIZE 1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define TOOLS_NUM 6
#define IN_BOARD 0

//WifiFuncs
void NTPsetup();
char* GetTimeString();
//RFIDFuncs
void ReadTagID(String* uid);
bool CheckUID(String displayed_card_id, String* name_ptr);
//OLEDFuncs
void InitializeOLED();
// ExpanderToolFuncs
// void i2c_init();
// void digitalWritePCF(uint8_t pcfAddress, int pin, bool value);
// bool digitalReadPCF(uint8_t pcfAddress, int pin);
void InitializeTools();
void UpdateUserTools(String& user_name, String& uid);
void WriteTool(int tool_num, int led_state);
bool ReadTool(int tool_num);
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
void ToolLoop();

//declare global variables
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_SDA);
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN); // Instance of the class
ezButton door_switch(SWITCH_PIN);
Adafruit_PCF8574 pcf1;
Adafruit_PCF8574 pcf2;

const char* wifi_ssid       = "Inon's";
const char* wifi_password   = "0502261118Ic";


unsigned long startTime; // Variable to store the start time in milliseconds
const unsigned long duration = 6 * 1000; // seconds * 1000 = duration in milliseconds

int general_tools[TOOLS_NUM]; // 0 - the tool is borrowed, 1 - tool is present in the board
int current_user_tools[TOOLS_NUM]; // 0 - the tool is not the user's, 1 - tool is borrowed by the user

enum CHANGE_IN_TOOLBOX {UNCHANGED, BORROWED, RETURNED};
enum TOOLS_IN_TOOLBOX {SCREW1, SCREW2, SCISSORS, PLIERS, SCREW3, SCREW4};

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

void setup() 
{
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  delay(1000);
  NTPsetup();
  delay(1000);
  InitSDCard();
  pcf1.begin(PCF8574_1_ADDRESS);
  pcf2.begin(PCF8574_2_ADDRESS);
  rfid.PCD_Init(); // Init MFRC522 
  door_switch.setDebounceTime(50);
  InitializeTools();
  delay(1000);           
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);  // initialize with the I2C addr 0x3D (for the 128x64)
  
  FillSDInitially();
  ReadSDInitially();
  Serial.println("Approximate your card to the reader...");
  // Clear the buffer.
  InitializeOLED();  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
}

void loop()
{
  if (!rfid.PICC_IsNewCardPresent()) 
  {
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
    Serial.println("Access denied");
    display.clearDisplay();
    display.setCursor(0,0); 
    display.print("Access denied");
    display.display();
    delay(2000);
    InitializeOLED();
    return;
  }
  int space_index = user_name.indexOf(' '); // Find the index of the first space
  String first_name = user_name.substring(0, space_index);
  while(!DoorChanged())
  {
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
     //add another 500 milliseconds of silence
    }
    ToolLoop();
  }
  digitalWrite(BUZZER_PIN, HIGH);
 //UPDATE USER'S TOOLS
  UpdateUserTools(user_name, displayed_card_id);
  // Serial.print ("user tools: "); //Debug info
  // Serial.println(*current_user_tools);
  UpdateLog(user_name);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Door is closed");
  digitalWrite(LED_PIN, LOW);  
  display.print("Goodbye ");
  display.println(first_name);
  display.display();
  delay(2000);
  InitializeOLED();
  return;
}