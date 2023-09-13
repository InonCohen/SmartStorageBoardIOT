/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
  Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
// #include <TelegramBot.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <string.h>

// Replace with your network credentials
const char* ssid = "Hila";
const char* password = "hilalevi";

// Initialize Telegram BOT
#define BOTtoken "6214411428:AAF8yQOTw0OyQ_O9o7AAl6cjddUneeJ3pz0"  // your Bot Token (Get from Botfather)
#define CS_PIN_SD 15

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1701605220"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
File myFile;

bool isMoreDataAvailable()
{
  return myFile.available();
}

byte getNextByte()
{
  return myFile.read();
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

void listDir(fs::FS &fs, const char * dirname, uint8_t levels, String chat_id)
{
    bot.sendMessage(chat_id, "Listing files:", "");

    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
      bot.sendMessage(chat_id, "Failed to open directory", "");
      Serial.println("Failed to open directory");
      return;
    }
    if(!root.isDirectory()){
      bot.sendMessage(chat_id, "Not a directory", "");
      Serial.println("Not a directory");
      return;
    }

    File file = root.openNextFile();
    while(file)
    {
        if(file.isDirectory()){
          // bot.sendMessage(chat_id, "  DIR : ", "");
          // bot.sendMessage(chat_id, file.name(), "");
          // Serial.print("  DIR : ");
          // Serial.println(file.name());
          // if(levels){
          //     listDir(fs, file.path(), levels -1, chat_id);
          // }
          bot.sendMessage(chat_id, "End of files ", "");
          return;
        } else {
          // bot.sendMessage(chat_id, "  FILE: ", "");
          bot.sendMessage(chat_id, file.name(), "");
          // bot.sendMessage(chat_id, "  SIZE: ", "");
          // bot.sendMessage(chat_id, file.size(), "");
          Serial.print("  FILE: ");
          Serial.print(file.name());
          Serial.print("  SIZE: ");
          Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void sendFile(fs::FS &fs, const char * path, String chat_id, const char * file_name)
{
  myFile = fs.open(path);
  if(!myFile){
      Serial.println("Failed to open file");
      return;
  }

  String sent = bot.sendMultipartFormDataToTelegram("sendDocument", "document", file_name, "csv/txt", chat_id, myFile.size(),
                                        isMoreDataAvailable,
                                        getNextByte, nullptr, nullptr);

  if (sent)
  {
    Serial.println("was successfully sent");
  }
  else
  {
    Serial.println("was not sent");
  }

  myFile.close();

}

void appendFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message))
  {
    Serial.println("Message appended");
  }
  else 
  {
    Serial.println("Append failed");
  }
  file.close();
}

void add_user(fs::FS &fs, String chat_id, char * text)
{
    char* token;
    //  = strtok(text, ",");
    /* get the first token */
    token = strtok(text, ",");
    Serial.println(token);
    String user_to_append = "";
    /* walk through other tokens */
    while( token != NULL ) {
        // printf( " %s\n", token );
        token = strtok(NULL, ",");
        Serial.println(token);
        user_to_append += token;
        user_to_append += ", ";
    }
    // Serial.println("1");
    // Serial.println(token);
    // String user_to_append = "";
    // token = strtok(NULL, ",");
    // Serial.println("2");
    // Serial.println(token);
    // String userID = token;
    // token = strtok(NULL, ",");
    // Serial.println("3");
    // Serial.println(token);
    // String userName = token;
    // Serial.println("4");
    // Serial.println(userID);
    // Serial.println(userName);
    // // String text = bot.messages[0].text;
    // // Serial.println(text);
    // user_to_append += userID;
    // user_to_append += ", ";
    // // bot.sendMessage(chat_id, "Please write user name", "");
    // // text = bot.messages[0].text;
    // // Serial.println(text);
    // user_to_append += userName;
    user_to_append += ", 0\n";
    Serial.println("5");
    Serial.println(user_to_append);
    char* user_to_append_char = (char*)malloc(user_to_append.length() + 1);
    strcpy(user_to_append_char, user_to_append.c_str());
    appendFile(SD, "/users.csv", user_to_append_char);
    Serial.println("User successfoly append");
    bot.sendMessage(chat_id, "User successfoly append", "");
}


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/list_dir to get all directory in SD \n";
      welcome += "/get_users to get users name \n";
      welcome += "/get_items_info to get items info \n";
      welcome += "/get_log to get board history \n";
      welcome += "/add_user , user_id , user_name to add user to the board \n";
      welcome += "/delete_user , user_id , user_name to remove user from the board \n";
      welcome += "/make_manager , user_id , user_name to make user a manager \n";
      welcome += "/add_item , item_id , item_name to add item to the board \n";
      welcome += "/delete_item , item_id , item_name to remove item from the board \n";
      bot.sendMessage(chat_id, welcome, "");
    }
  

    if (text == "/list_dir") {
      listDir(SD, "/", 0, chat_id);
    }
    
    if (text == "/get_users") {
      sendFile(SD, "/users.csv", chat_id, "users.csv");
    }
    
    if (text == "/get_items_info") {
      sendFile(SD, "/items.csv", chat_id, "items.csv");
    }

    if (text == "/get_log") {
      sendFile(SD, "/log.txt", chat_id, "log.txt");
    }
    char* tmp_text = (char*)malloc(text.length() + 1);
    strcpy(tmp_text, text.c_str());

    String order = strtok(tmp_text, ",");
    if (order == "/add_user") 
    {
      add_user(SD, chat_id, tmp_text);
    }
  }
}

void setup() {
  Serial.begin(115200);
  InitSDCard();
  SPI.begin(); // Init SPI bus
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}