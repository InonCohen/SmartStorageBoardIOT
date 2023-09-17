void ToolLoop(){
  // Serial.println("In ToolLoop");
  //tool_switch.loop(); // MUST call the loop() function first
  //Determine state of tool
  String switchesFirstState[TOOLS_NUM];
  // String switch2FirstState[3];
  String switchesLastState[TOOLS_NUM];
  // String switch2LastState[3];
  for (int j = 0; j < TOOLS_NUM; j++) {
    switchesFirstState[j] = ReadTool(j) ? "OFF" : "ON";
  }

  //determine leds of tools
  for (int i = 0 ; i < TOOLS_NUM ; i++)
  {
    // Serial.println("In for loop");
    // Serial.print("i = ");
    // Serial.println(i);
    // Serial.println(current_user_tools[i]);
    if(current_user_tools[i] == 1 || general_tools[i] == 1 ){// if the tool is registered as the user's or is free
      WriteTool(i, HIGH);
    }
    else{
      WriteTool(i, LOW);
    }
  }

  //read tools states
  for (int j = 0; j < 3; j++) {
    WriteTool(j, 1);
    WriteTool(j+3, 1);
  }
  delay(1);
  for (int j = 0; j < TOOLS_NUM; j++) {
    switchesLastState[j] = ReadTool(j) ? "OFF" : "ON";
    if (switchesFirstState[j] == "OFF") {
      WriteTool(j, 0);
    }
  }
  for (int j = 0; j < TOOLS_NUM; j++){
    if(switchesFirstState[j] != switchesLastState[j]){
      if(switchesLastState[j] == "OFF"){
        tools_condition[j] = BORROWED;
      }
      else{
        tools_condition[j] = RETURNED;
      }
    }
  }
}

//update the general_tools array, and zero the rest of the arrays to be ready for a new user
void UpdateTools()
{
  //update general_tools by the tools file
  //update all current_user to UNCHANGED
  for (int i = 0 ; i < TOOLS_NUM ; i++) 
  {
    current_user_tools[i] = 0;
    tools_condition[i] = UNCHANGED;
    ReadItems(general_tools); 
  }
}

//update current_user_tools after a visit to the toolbox
void UpdateUserTools(String& user_name, String& uid)
{
  for (int i = 0 ; i < TOOLS_NUM ; i++)
  {
    if(tools_condition[i] != UNCHANGED)
    {
      int old = current_user_tools[i];
      current_user_tools[i] = (tools_condition[i] == BORROWED);
      if (old == current_user_tools[i])
      {
        tools_condition[i] = UNCHANGED;
      }
      else
      {
        // Serial.println("changing general array");
        // Serial.println(general_tools[i]);
        general_tools[i] = 1 - current_user_tools[i];
        if(general_tools[i] == 1)
        {
          UpdateItem(i);
        }
        else
        {
          UpdateItem(i, uid);
        }
        // Serial.println(general_tools[i]);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Door is closed"); 
        display.println(user_name);
        String tool_change_str = tools_change_strings[tools_condition[i]];
        tool_change_str.toLowerCase();
        display.print(tool_change_str);
        display.println(" the tool");
        display.display();
        delay(2500);
      }
    }
  }
}

void WriteTool(int tool_num, int led_state){
  if(tool_num>=0 && tool_num<3){
    pcf1.digitalWrite(2*tool_num, led_state);
    return;
  }
  if(tool_num>=3 && tool_num<6){
    pcf2.digitalWrite(2*(tool_num-3), led_state);
  }
}

bool ReadTool(int tool_num){
  if(tool_num>=0 && tool_num<3){
    return pcf1.digitalRead(2*tool_num+1);
  }
  return pcf2.digitalRead(2*(tool_num-3)+1);
}

