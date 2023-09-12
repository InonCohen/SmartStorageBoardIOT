void ToolLoop(){
  // Serial.println("In ToolLoop");
  tool_switch.loop(); // MUST call the loop() function first
  for (int i = 0 ; i < TOOLS_NUM ; i++)
  {
    // Serial.println("In for loop");
    // Serial.print("i = ");
    // Serial.println(i);
    // Serial.println(current_user_tools[i]);
    if(current_user_tools[i] == 1 || general_tools[i] == 1 ){// if the tool is registered as the user's or is free
        if (tool_switch.isPressed()){
          // Serial.println("The tool switch: ON -> OFF");
          Serial.println("The tool is RETURNED");
          tools_condition[i] = RETURNED;
          //current_user_tools[i] = 1;
          digitalWrite(LED_PIN, LOW);   //LOW for led is off
        }
        if (tool_switch.isReleased()){
          // Serial.println("The tool switch: OFF -> ON");
          Serial.println("The tool is BORROWED");
          tools_condition[i] = BORROWED;
          //current_user_tools[i] = 0;
          digitalWrite(LED_PIN, HIGH);  //HIGH for led is on
        }
        int state = tool_switch.getState();
        if (state == HIGH) // state is for the tool switch, HIGH is not pressed
        {
          // digitalWrite(LED_PIN, LOW); 
          Serial.println("The tool is MISSING");
        }
        else // state is for the tool switch, LOW is pressed
        {
          digitalWrite(LED_PIN, HIGH);  
          Serial.println("The tool is HERE");
        }
        delay(200);
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