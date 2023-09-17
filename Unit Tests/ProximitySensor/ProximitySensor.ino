//Written by Nick Koumaris
//info@educ8s.tv

#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ezButton.h>
#include "FS.h"
#include "SD.h"

#define TOOL_PIN 32
#define LED_PIN 2
#define TOOLS_NUM 1
ezButton tool_switch(TOOL_PIN);


void setup() 
{
  Serial.begin(115200);
  tool_switch.setDebounceTime(50);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);           

}

void loop(){
   tool_switch.loop(); // MUST call the loop() function first
  for (int i = 0 ; i < TOOLS_NUM ; i++)
  {
if (tool_switch.isPressed()){
          // Serial.println("The tool switch: ON -> OFF");
          Serial.println("The tool is RETURNED");

          digitalWrite(LED_PIN, LOW);   //LOW for led is off
        }
        if (tool_switch.isReleased()){
          // Serial.println("The tool switch: OFF -> ON");
          Serial.println("The tool is BORROWED");
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