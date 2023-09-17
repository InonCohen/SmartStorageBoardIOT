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
// #include <MFRC522.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
#include <stdio.h>
#include <stdlib.h>
// #include <ezButton.h>
// #include "time.h"
// #include "sntp.h"

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


bool CheckID(String displayed_card_id, String* name_ptr, const char * file_name)
{
  Serial.println();
  Serial.print("Message : ");
  displayed_card_id.toUpperCase();
  char* file_content = (char*)malloc(100);
  File file = SD.open(file_name);
  bool authorized = false;
  while(readLine(file, file_content, file.size()))
  {
    char* token = strtok(file_content, ",");
    char *ID;
    if (token != NULL) 
    {
    // Extract the content of the first item (column) into uid
      ID = token;
    }
    if (displayed_card_id == ID) //change here the UID of the card/cards that you want to give access
    {
        // Move to the next token (second column)
      token = strtok(NULL, ",");
      char* Name = token;
      String name_string(Name);
      name_string.trim();
      *name_ptr = name_string;
      // Serial.print("user name is ");
      // Serial.println(userName);
      // Serial.println("Authorized access");
      // display.clearDisplay();
      // display.setCursor(0, 0);
      // display.print("Hello");
      // display.print(userName);
      // display.display();
      authorized = true;
      Serial.println();
      delay(500);
      break;
    }
  }
  return authorized;
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


void add_new(fs::FS &fs, String chat_id, String text, const char * file_name)
{
    char* token;
    char text_char[text.length() + 1];
    strcpy(text_char, text.c_str());
    token = strtok(text_char, ",");
    String order(token);
    order.trim();
    String to_append = "";
    /* walk through other tokens */
    token = strtok(NULL, ",");
    String ID(token);
    ID.trim();
    if(ID == NULL)
    {
      Serial.println("order was not writen in correct way, try again");
      bot.sendMessage(chat_id, "order was not writen in correct way, try again", "");  
      return;
    }
    to_append += ID;
    to_append += ", ";
    token = strtok(NULL, ",");
    String Name(token);
    Name.trim();
    if(Name == NULL)
    {
      Serial.println("order was not writen in correct way, try again");
      bot.sendMessage(chat_id, "order was not writen in correct way, try again", "");  
      return;
    }
    to_append += Name;
    Serial.println(to_append);
    token = strtok(NULL, ",");
    if(token != NULL)
    {
      Serial.println("order was not writen in correct way, try again");
      bot.sendMessage(chat_id, "order was not writen in correct way, try again", "");  
      return;
    }
    if(CheckID(ID, &Name, file_name))
    {
      Serial.println("Already exist");
      Serial.println(to_append);
      bot.sendMessage(chat_id, "Already exist", "");  
      return;
    }
    to_append += ", 0\n";
    char* to_append_char = (char*)malloc(to_append.length() + 1);
    strcpy(to_append_char, to_append.c_str());
    appendFile(SD, file_name, to_append_char);
    Serial.println("Successfoly append");
    Serial.println(to_append);
    bot.sendMessage(chat_id, "Successfoly append", "");
}

void delete_from_file(fs::FS &fs, const char* file_name, String text, String chat_id, String to_change)
{

  char* token;
  char text_char[text.length() + 1];
  strcpy(text_char, text.c_str());
  token = strtok(text_char, ",");
  String order(token);
  order.trim();
  String to_append = "";
  /* walk through other tokens */
  token = strtok(NULL, ",");
  String displayed_card_id(token);
  displayed_card_id.trim();
  if(displayed_card_id == NULL)
  {
    Serial.println("order was not writen in correct way, try again");
    bot.sendMessage(chat_id, "order was not writen in correct way, try again", "");  
    return;
  }
  to_append += displayed_card_id;
  to_append += ", ";
  token = strtok(NULL, ",");
  String Name(token);
  Name.trim();
  if(Name == NULL)
  {
    Serial.println("order was not writen in correct way, try again");
    bot.sendMessage(chat_id, "order was not writen in correct way, try again", "");  
    return;
  }
  to_append += Name;
  token = strtok(NULL, ",");
  if(token != NULL)
  {
    Serial.println("order was not writen in correct way, try again");
    bot.sendMessage(chat_id, "order was not writen in correct way, try again", "");  
    return;
  }
  // Serial.println(displayed_card_id);
	// Open the original CSV file for reading
	File originalFile = fs.open(file_name, FILE_READ);
		
	// Create a new CSV file for writing
	File newFile = fs.open("/temp.csv", FILE_WRITE);

	char* file_content = (char*)malloc(100);
  // Check if both files opened successfully
  if (originalFile && newFile) 
  {
     while (originalFile.available()) {
      String line = originalFile.readStringUntil('\n');
      char line_char[line.length() + 1];
      strcpy(line_char, line.c_str());
      char* token = strtok(line_char, ",");
      char *ID;
      if (token != NULL) 
      {
      // Extract the content of the first item (column) into uid
        ID = token;
      }
      if (displayed_card_id != ID) //change here the UID of the card/cards that you want to give access
      {
        // String line = origin_line.readStringUntil('\n');

        line.trim();
        Serial.println(line);
        newFile.println(line);
      }
      else if (to_change != "")
      {
        to_append += to_change;
        Serial.println(to_append);
        newFile.println(to_append);
      }
    }


    // while(readLine(originalFile, file_content, originalFile.size()))
    // {
    //   originalFile
    //   String origin_line = file_content;
    //   char* token = strtok(file_content, ",");
    //   char *ID;
    //   if (token != NULL) 
    //   {
    //   // Extract the content of the first item (column) into uid
    //     ID = token;
    //   }
    //   if (displayed_card_id != ID) //change here the UID of the card/cards that you want to give access
    //   {
    //     // String line = origin_line.readStringUntil('\n');
    //     newFile.println(origin_line);
    //   }
    // }
      // Close both files
    originalFile.close();
    newFile.close();

    // Remove the original file
    fs.remove(file_name);

    // Rename the new file to the original file's name
    fs.rename("/temp.csv", file_name);
    
    bot.sendMessage(chat_id, "Line deleted successfully.", "");
    Serial.println("Line deleted successfully.");
  }
  else {
    bot.sendMessage(chat_id, "Error opening files.", "");
    Serial.println("Error opening files");
  }
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
    text.trim();
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
      welcome += "/remove_manager , user_id , user_name to remove user from managing \n";
      welcome += "/add_item , item_id , item_name to add item to the board \n";
      welcome += "/delete_item , item_id to remove item from the board \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    else if (text == "/list_dir") {
      listDir(SD, "/", 0, chat_id);
    }
    else if (text == "/get_users") {
      sendFile(SD, "/users.csv", chat_id, "users.csv");
    }
    else if (text == "/get_items_info") {
      sendFile(SD, "/items.csv", chat_id, "items.csv");
    }
    else if (text == "/get_log") {
      sendFile(SD, "/log.txt", chat_id, "log.txt");
    }
    else
    {
      char* tmp_text = (char*)malloc(text.length() + 1);
      strcpy(tmp_text, text.c_str());
      String order = strtok(tmp_text, ",");
      order.trim();
      Serial.println(order);
      if (order == "/add_user") 
      {
        add_new(SD, chat_id, text, "/users.csv");
      }
      else if (order == "/add_item")
      {
        add_new(SD, chat_id, text, "/items.csv");
      }
      else if (order == "/make_manager")
      {
        // UpdateFile(text, "1", "/users.csv", chat_id);
        delete_from_file(SD, "/users.csv", text, chat_id, ", 1");
      }
      else if (order == "/remove_manager")
      {
        // UpdateFile(text, "1", "/users.csv", chat_id);
        delete_from_file(SD, "/users.csv", text, chat_id, ", 0");
      }
      else if(order == "/delete_item")
      {
        delete_from_file(SD, "/items.csv", text, chat_id, "");
      }
      else if(order == "/delete_user")
      {
        delete_from_file(SD, "/users.csv", text, chat_id, "");
      }
      else
      {
        Serial.println("Don't know this order, please try again");
        bot.sendMessage(chat_id, "Don't know this order, please try again", "");  
      }
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