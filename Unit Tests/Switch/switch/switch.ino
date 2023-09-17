/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-switch
 */

#include <ezButton.h>

unsigned long startTime; // Variable to store the start time in milliseconds
unsigned long duration = 0.1 * 60 * 1000; // Two minutes in milliseconds
int buzzer=5;
ezButton mySwitch(22);  // create ezButton object that attach to ESP32 pin GPIO17

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  mySwitch.setDebounceTime(50); // set debounce time to 50 milliseconds
  startTime=0;
}

void loop() {
  mySwitch.loop(); // MUST call the loop() function first

  if (mySwitch.isPressed()){
    int endTime = startTime + 1000;
    if(startTime == 0){
      Serial.println("The switch: OFF -> ON");
      startTime = millis();
      endTime = startTime + duration;
    }
    if(startTime>=endTime){
      tone(buzzer,2000,700);
     //add another 500 milliseconds of silence

     tone(buzzer,0,200);
    }
  }

  if (mySwitch.isReleased())
    Serial.println("The switch: ON -> OFF");
    noTone(buzzer);
    startTime = 0;

  int state = mySwitch.getState();
  // if (state == HIGH)
  //   Serial.println("The switch: OFF");
  // else
  //   Serial.println("The switch: ON");
}