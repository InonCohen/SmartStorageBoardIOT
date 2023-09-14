#include <Wire.h>
#include <Adafruit_PCF8574.h>
#include "driver/i2c.h"

#define PCF8574_1_ADDRESS 0x20
#define PCF8574_2_ADDRESS 0x27

// // Define the SDA and SCL pins
// #define I2C_SDA_PIN 26
// #define I2C_SCL_PIN 27

// bool switchPressed[3] = { 0, 0, 0 };

#define LOOP_LENGTH 20
#define DELAY_TIME 300

Adafruit_PCF8574 pcf1;
Adafruit_PCF8574 pcf2;
String states[8][3] = { { "OFF", "OFF", "OFF" }, { "OFF", "OFF", "ON" }, { "OFF", "ON", "OFF" }, { "OFF", "ON", "ON" }, { "ON", "OFF", "OFF" }, { "ON", "OFF", "ON" }, { "ON", "ON", "OFF" }, { "ON", "ON", "ON" } };
void setup() {
  Serial.begin(115200);
  Wire.begin();
  // i2c_init();
  pcf1.begin(PCF8574_1_ADDRESS, &Wire);
  pcf2.begin(PCF8574_2_ADDRESS, &Wire);
  delay(500);
  for (int i = 0; i < 3; i++) {
    pcf1.pinMode(2 * i + 1, INPUT);
    pcf1.pinMode(2 * i, OUTPUT);
    pcf2.pinMode(2 * i + 1, INPUT);
    pcf2.pinMode(2 * i, OUTPUT);
  }
}

void loop() {

  for (int iter = 0; iter < 8; iter++) {
    Serial.printf("Led1 %s Led2 %s Led3 %s:\n", states[iter][0], states[iter][1], states[iter][2]);
    for (int i = 0; i < LOOP_LENGTH; i++) {
      //digitalWritePCF(PCF8574_1_ADDRESS, 0, 1);                 //turn on led unit 0
      for (int j = 0; j < 3; j++) {
        pcf1.digitalWrite(2 * j, 1);
        pcf2.digitalWrite(2 * j, 1);

      }
      delay(1);
      String switch1State[3];
      String switch2State[3];

      for (int j = 0; j < 3; j++) {
        switch1State[j] = pcf1.digitalRead(2 * j + 1) ? "OFF" : "ON";
        switch2State[j] = pcf2.digitalRead(2 * j + 1) ? "OFF" : "ON";

        if (states[iter][j] == "OFF") {
          pcf1.digitalWrite(2 * j, 0);
          pcf2.digitalWrite(2 * j, 0);

        }
        Serial.printf("LED%d - %s  Switch%d - %s    ", j + 1, states[iter][j], j + 1, switch1State[j]);
        Serial.printf("LED%d - %s  Switch%d - %s    ", j + 4, states[iter][j], j + 4, switch2State[j]);

      }
      Serial.print("\n");
      delay(DELAY_TIME);
    }
    Serial.print("\n");

  }
}



