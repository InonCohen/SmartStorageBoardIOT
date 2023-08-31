/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-switch
 */

#include <ezButton.h>

#define SWITCH_PIN 32
#define LED_PIN 2

ezButton mySwitch(SWITCH_PIN);  // create ezButton object that attach to ESP32 pin GPIO17

void setup() {
  Serial.begin(115200);
  mySwitch.setDebounceTime(50); // set debounce time to 50 milliseconds
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  mySwitch.loop(); // MUST call the loop() function first

  if (mySwitch.isPressed())
    Serial.println("The switch: OFF -> ON");

  if (mySwitch.isReleased())
    Serial.println("The switch: ON -> OFF");

  int state = mySwitch.getState();
  if (state == HIGH)
    Serial.println("The switch: OFF");
  else
    Serial.println("The switch: ON");

  digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);           
}