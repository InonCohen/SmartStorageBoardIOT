
void InitSDCard(){
  if(!SD.begin(SD_CS_PIN)){
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

void listDir(fs::FS &fs, const char * dirname, uint8_t levels, String chat_id = ""){
  if(chat_id != "")
  {
    bot.sendMessage(chat_id, "Listing files:", "");
  }
  Serial.printf("Listing directory: %s\n", dirname);
  
  File root = fs.open(dirname);
  if(!root){
      if(chat_id != "")
      {
        bot.sendMessage(chat_id, "Failed to open directory", "");
      }
      Serial.println("Failed to open directory");
      return;
  }
  if(!root.isDirectory()){
      if(chat_id != "")
      {
        bot.sendMessage(chat_id, "Not a directory", "");
      }
      Serial.println("Not a directory");
      return;
  }
  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
        if(chat_id == "")
        {
          Serial.print("  DIR : ");
          Serial.println(file.name());
          if(levels){
              listDir(fs, file.path(), levels -1);
          }
        }
        else
        {
          bot.sendMessage(chat_id, "End of files ", "");
          return;
        } 
    } else {
        if(chat_id != "")
        {
          bot.sendMessage(chat_id, file.name(), "");
        }
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

void readFile(fs::FS &fs, const char * path, bool read_headers)
{
  Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file)
  {
      Serial.println("Failed to open file for reading");
      return;
  }
  // Serial.println("Read from file:");
  if(!read_headers){
    if (file.available()){
      file.readStringUntil('\n');
    }
  }
  while(file.available())
  {
      Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file)
  {
      Serial.println("Failed to open file for writing");
      return;
  }
  if(file.print(message))
  {
      Serial.println("File written");
  } else 
  {
      Serial.println("Write failed");
  }
  file.close();
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

void deleteFile(fs::FS &fs, const char * path)
{
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path))
  {
    Serial.println("File deleted");
  } 
  else 
  {
    Serial.println("Delete failed");
  }
}

void ReadSDInitially()
{
  readFile(SD, "/users.csv", true);
  readFile(SD, "/items.csv", true);
  readFile(SD, "/log.txt", true);
}

void FillSDInitially()
{
  // listDir(SD, "/", 0);
  clearSD();
  // listDir(SD, "/", 0);
  writeFile(SD, "/users.csv", "User ID, Name, Privilege Level\n");
  appendFile(SD, "/users.csv", "F9 CF C4 A3, Hila Levi, 0\n");
  appendFile(SD, "/users.csv", "59 C5 23 A4, Mais Fadila, 1\n");
  writeFile(SD, "/items.csv", "Borrower ID\n");
  appendFile(SD, "/items.csv", "0\n0\n0\n0\n0\n0\n");
  String log_str = "Initialized ";
  char* time_str = "";
  if(!need_reconnection){
    char* time_str = GetTimeString();
    log_str += time_str;
    free(time_str);
  }
  log_str += "\n";
  writeFile(SD, "/log.txt", log_str.c_str());
  listDir(SD, "/", 0);
}

void UpdateItem(int line_number, String uid){
  if(uid == "")
  {
    uid = String(IN_BOARD);
  }
  // Serial.print("Updating the tool on line ");
  // Serial.print(line_number);
  // Serial.print(" with uid ");
  // Serial.println(uid);
  readFile(SD, "/items.csv", true);
  File file = SD.open("/items.csv", FILE_READ);
  // Serial.println("after opening the file");
  // readFile(SD, "/items.csv", true);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  // File new_file = SD.open("\tmp.csv", FILE_WRITE);
  String line; // Assuming a maximum line length of 100 characters
  int current_line = 0;
  // Loop through the file line by line
  if (file.available()) {
    // Serial.println("File is available");
    line = file.readStringUntil('\n');
    writeFile(SD, "/tmp.csv", line.c_str());
    appendFile(SD,"/tmp.csv", "\n");
    Serial.print("Line is: ");
    Serial.println(line);    
  for(; current_line < line_number ; current_line++) {
    line = file.readStringUntil('\n');
    Serial.print("Line is: ");
    Serial.println(line);
    appendFile(SD, "/tmp.csv", line.c_str());
    appendFile(SD, "/tmp.csv", "\n");
    }
    line = file.readStringUntil('\n');
    // Serial.println(current_line);
    // Write the new content (IN_BOARD value)
    appendFile(SD, "/tmp.csv", uid.c_str());
    appendFile(SD, "/tmp.csv", "\n");
    while(file.available()){
      line = file.readStringUntil('\n');
      appendFile(SD, "/tmp.csv", line.c_str());
      appendFile(SD, "/tmp.csv", "\n");
    }
  file.close();
  deleteFile(SD, "/items.csv");// readFile(SD, "/items.csv");
  SD.rename("/tmp.csv", "/items.csv");
  readFile(SD, "/items.csv", true);
}
}

void ReadItems(int arr[], const String& const_uid) {
  // Initialize arr to all zeros
  // Serial.println("In ReadItems");
  // Serial.print("const_uid is ");
  // Serial.println(const_uid);
  String uid = const_uid;
  uid.trim();
  // Serial.print("uid == const_uid ? ");
  // Serial.println(uid == const_uid);
  for (int i = 0 ; i < TOOLS_NUM ; i++) 
  {
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
  if (file.available()){
    line = file.readStringUntil('\n'); // skip headers line
    line.trim();
  }
  // Read each line from the file
  while (file.available()) {
    // Serial.print("line ");
    // Serial.print(lineNumber);
    // Serial.println(" reads as follows: ");
    line = file.readStringUntil('\n');
    line.trim();
    // Serial.println(line);
    lineNumber++;
    // Compare the line with uid
    if (lineNumber <= TOOLS_NUM) {
      // Serial.print("uid == '' ? ");
      // Serial.println(uid == "");
      // Set the corresponding element of arr to 1
      arr[lineNumber-1] = (uid == "") ? (line == String(IN_BOARD)) : line.equals(uid);
    }
  }  
  // Close the file
  file.close();
}

void UpdateLog(String& user_name)
{
  char* timeString = "";
  if(!need_reconnection){
    timeString = GetTimeString();
  }
  for (int i = 0 ; i < TOOLS_NUM ; i++)
  {
    if(tools_condition[i] != UNCHANGED) 
    {
        String log_txt = tools_in_toolbox[i];
        log_txt += " is ";
        if(tools_condition[i] == RETURNED)
        {
          log_txt += "returned by ";
        }
        else
        {
          log_txt += "borrowed by ";
        }
        log_txt += user_name.c_str();
        if(!need_reconnection){
          log_txt += " on ";
          log_txt += timeString;
        }
        log_txt += "\n";
        appendFile(SD, "/log.txt", log_txt.c_str());
        
    }
  }
  free(timeString);
}