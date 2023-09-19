

File myFile;

bool isMoreDataAvailable()
{
  return myFile.available();
}

byte getNextByte()
{
  return myFile.read();
}


void sendFile(fs::FS &fs, const char * path, String chat_id, const char * file_name)
{
  myFile = fs.open(path);
  if(!myFile){
    bot.sendMessage(chat_id, "Failed to open file", "");
    Serial.println("Failed to open file");
    return;
  }

  String sent = bot.sendMultipartFormDataToTelegram("sendDocument", "document", file_name, "csv/txt", chat_id, myFile.size(),
                                        isMoreDataAvailable,
                                        getNextByte, nullptr, nullptr);

  if (sent)
  {
    Serial.println("File was successfully sent");
  }
  else
  {
    bot.sendMessage(chat_id, "Failed to send file", "");
    Serial.println("Failed to send file");
  }
  myFile.close();
}

void add_new_user(fs::FS &fs, String chat_id, String text, const char * file_name)
{
    char* token;
    char text_char[text.length() + 1];
    strcpy(text_char, text.c_str());
    token = strtok(text_char, ",");
    String command(token);
    command.trim();
    String to_append = "";
    /* walk through other tokens */
    token = strtok(NULL, ",");
    String ID(token);
    ID.trim();
    if(ID == NULL)
    {
      Serial.println("Command was not written correctly: UserID and UserName are missing");
      bot.sendMessage(chat_id, "Command was not written correctly, the structure is '/add_user, UserID, UserName'", "");  
      return;
    }
    to_append += ID;
    to_append += ", ";
    token = strtok(NULL, ",");
    String Name(token);
    Name.trim();
    if(Name == NULL)
    {
      Serial.println("Command was not written correctly: UserName is missing");
      bot.sendMessage(chat_id, "Command was not written correctly, the structure is '/add_user, UserID, UserName'", "");  
      return;
    }
    to_append += Name;
    Serial.println(to_append);
    token = strtok(NULL, ",");
    if(token != NULL)
    {
      Serial.println("Command was not written correctly, the structure is 'command, UserID, UserName'");
      bot.sendMessage(chat_id, "Command was not written correctly, the structure is '/add_user, UserID, UserName'", "");  
      return;
    }
    if(CheckUID(ID, &Name, false))
    {
      Serial.println("User already exists");
      Serial.println(to_append);
      bot.sendMessage(chat_id, "User already exists", "");  
      return;
    }
    to_append += ", 0\n";
    char* to_append_char = (char*)malloc(to_append.length() + 1);
    strcpy(to_append_char, to_append.c_str());
    appendFile(SD, file_name, to_append_char);
    Serial.println("User successfully appended");
    Serial.println(to_append);
    bot.sendMessage(chat_id, "User successfully appended", "");
}

void change_file(fs::FS &fs, String text, String chat_id, String to_change)
{

  char* token;
  char text_char[text.length() + 1];
  strcpy(text_char, text.c_str());
  token = strtok(text_char, ",");
  String command(token);
  command.trim();
  String to_append = "";
  /* walk through other tokens */
  token = strtok(NULL, ",");
  String userID(token);
  userID.trim();
  if(userID == NULL)
  {
    Serial.println("Command was not written correctly, the structure is 'command, UserID, UserName'");
    bot.sendMessage(chat_id, "Command was not written correctly, the structure is 'command, UserID, UserName'", "");  
    return;
  }
  to_append += userID;
  to_append += ", ";
  token = strtok(NULL, ",");
  String Name(token);
  Name.trim();
  if(Name == NULL)
  {
    Serial.println("Command was not written correctly, the structure is 'command, UserID, UserName'");
    bot.sendMessage(chat_id, "Command was not written correctly, the structure is 'command, UserID, UserName'", "");  
    return;
  }
  to_append += Name;
  token = strtok(NULL, ",");
  if(token != NULL)
  {
    Serial.println("Command was not written correctly, the structure is 'command, UserID, UserName'");
    bot.sendMessage(chat_id, "Command was not written correctly, the structure is 'command, UserID, UserName'", "");  
    return;
  }
  // Serial.println(userID);
	// Open the original CSV file for reading
	File originalFile = fs.open("/users.csv", FILE_READ);
		
	// Create a new CSV file for writing
	File newFile = fs.open("/temp.csv", FILE_WRITE);

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
      if (userID != ID) //change here the UID of the card/cards that you want to give access
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
      // Close both files
    originalFile.close();
    newFile.close();

    // Remove the original file
    fs.remove("/users.csv");

    // Rename the new file to the original file's name
    fs.rename("/temp.csv", "/users.csv");
    if(to_change == "")
    {
      bot.sendMessage(chat_id, "User deleted successfully", "");
      Serial.println("User deleted successfully");
    }
    else 
    {
      bot.sendMessage(chat_id, "User status successfully changed", "");
      Serial.println("User status successfully changed");
    }

  }
  else {
    bot.sendMessage(chat_id, "Error opening file", "");
    Serial.println("Error opening file");
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
      welcome += "Use the following commands to control your board:\n\n";
      welcome += "'/list_files' - get all files on the SD card \n";
      welcome += "'/get_users' - get all user names \n";
      welcome += "'/get_tools' - get tools information \n";
      welcome += "'/get_log' - get tools-lending history \n";
      welcome += "'/add_user , user_id , user_name' - add a new user to the board \n";
      welcome += "'/delete_user , user_id , user_name' - remove a user from the board \n";
      welcome += "'/make_manager , user_id , user_name' - allow a user to manage the board \n";
      welcome += "'/remove_manager , user_id , user_name' - revoke a user's managing privileges \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    else if (text == "/list_files") {
      listDir(SD, "/", 0, chat_id);
    }
    else if (text == "/get_users") {
      sendFile(SD, "/users.csv", chat_id, "users.csv");
    }
    else if (text == "/get_tools") {
      sendFile(SD, "/items.csv", chat_id, "items.csv");
    }
    else if (text == "/get_log") {
      sendFile(SD, "/log.txt", chat_id, "log.txt");
    }
    else
    {
      char* tmp_text = (char*)malloc(text.length() + 1);
      strcpy(tmp_text, text.c_str());
      String command = strtok(tmp_text, ",");
      command.trim();
      Serial.println(command);
      if (command == "/add_user") 
      {
        add_new_user(SD, chat_id, text, "/users.csv");
      }
      // else if (command == "/add_item")
      // {
      //   add_new(SD, chat_id, text, "/items.csv");
      // }
      else if (command == "/make_manager")
      {
        // UpdateFile(text, "1", "/users.csv", chat_id);
        change_file(SD, text, chat_id, ", 1");
      }
      else if (command == "/remove_manager")
      {
        // UpdateFile(text, "1", "/users.csv", chat_id);
        change_file(SD, text, chat_id, ", 0");
      }
      // else if(command == "/delete_item")
      // {
      //   change_file(SD, "/items.csv", text, chat_id, "");
      // }
      else if(command == "/delete_user")
      {
        change_file(SD, text, chat_id, "");
      }
      else
      {
        Serial.println("Don't know this command, please try again");
        bot.sendMessage(chat_id, "Don't know this command, please try again", "");  
      }
    }
    
  }
}

void telegram_loop() {
  Serial.println("here 1");
  // if (millis() > lastTimeBotRan + botRequestDelay)  
  // {
      Serial.println("here 2");

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.println("here 3");
    Serial.println(numNewMessages);
    if(numNewMessages) {
        Serial.println("here 4");

      Serial.println("got response");
      handleNewMessages(numNewMessages);
      //numNewMessages = bot.getUpdates(bot.last_message_received);
    }
    lastTimeBotRan = millis();
  // }
}