

void ReadTagID(String* uid)
{
    //Show UID on serial monitor and on OLED
  Serial.print("UID tag :");
  byte letter;
  //fill displayed_card_id
  for (byte i = 0 ; i < rfid.uid.size ; i++) 
  {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    (*uid).concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    (*uid).concat(String(rfid.uid.uidByte[i], HEX));
  }
  (*uid).toUpperCase();
}

bool CheckUID(String displayed_card_id, String* name_ptr, bool print_to_OLED)
{
  Serial.println();
  Serial.print("Message : ");
  displayed_card_id.toUpperCase();
  char* file_content = (char*)malloc(100);
  File file = SD.open("/users.csv");
  bool authorized = false;
  file.readStringUntil('\n');
  while(readLine(file, file_content, file.size()))
  {
    char* token = strtok(file_content, ",");
    char *userID;
    if (token != NULL) 
    {
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
      if(print_to_OLED)
      {
        Serial.print("user name is ");
        Serial.println(userName);
        Serial.println("Authorized access");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Hello");
        display.print(userName);
        display.display();
      }
      authorized = true;
      Serial.println();
      delay(1500);
      break;
    }
  }
  return authorized;
}
