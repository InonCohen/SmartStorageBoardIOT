void ToolLoop(){
  // Serial.println("In ToolLoop");
  //tool_switch.loop(); // MUST call the loop() function first
  //Determine state of tool
  // String switch2FirstState[3];
  // String switch2LastState[3];

  //determine leds of tools
  for (int i = 0 ; i < TOOLS_NUM ; i++)
  {
    if(current_user_tools[i] == 1 || general_tools[i] == 1 ){// if the tool is registered as the user's or is free
      WriteTool(i, HIGH);
    }
    else{
      WriteTool(i, LOW);
    }
  }
  delay(300);
  // for (int i = 0; i < TOOLS_NUM; i++) {
  //   if(current_user_tools[i] == 1 || general_tools[i] == 1 ){
  //     switchesLastState[i] = ReadTool(i) ? "OFF" : "ON";
  //     if (switchesFirstState[i] == "OFF") {
  //       WriteTool(i, 1);
  //     }
  //   }
  // }
  // delay(500);
  // Serial.println("Switches conditions last:");
  // for (int j = 0; j < TOOLS_NUM; j++) {
  //   Serial.print(switchesLastState[j]);
  // }
  // Serial.print("\n");

}

//update the general_tools array, and zero the rest of the arrays to be ready for a new user
void UpdateTools()
{
  //update general_tools by the tools file
  //update all current_user to UNCHANGED
  ReadItems(general_tools); 
  for (int i = 0 ; i < TOOLS_NUM ; i++) 
  {
    current_user_tools[i] = 0;
    tools_condition[i] = UNCHANGED;
  }
}

//update current_user_tools after a visit to the toolbox
void UpdateUserTools(String& user_name, String& uid)
{
  for (int i = 0 ; i < TOOLS_NUM ; i++)
  {
    if((tools_condition[i] != UNCHANGED))//(current_user_tools[i] == 1 || general_tools[i] == 1) &&
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
        display.print(" ");
        display.println(tools_in_toolbox[i]);
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

void TurnOffBoard(){
  for (int i = 0 ; i< TOOLS_NUM ; i++)
    WriteTool(i, 0);
}

void TurnOnBoard(){
  for (int i = 0 ; i< TOOLS_NUM ; i++)
    WriteTool(i, 1);
}

void GetSwitchesState(String* arr){
  TurnOnBoard();
  for (int j = 0; j < TOOLS_NUM; j++) {
    arr[j] = ReadTool(j) ? "OFF" : "ON";
  }
  TurnOffBoard();
}