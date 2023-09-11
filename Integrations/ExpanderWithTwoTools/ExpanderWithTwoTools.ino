#include <Wire.h>

#define PCF8574_1_ADDRESS 0x20
#define PCF8574_2_ADDRESS 0x27


void setup() {
  Wire.begin();
}

void loop() {
  // Read the state of proximity switch for PCF8574 #1
  bool switch1Pressed = digitalReadPCF(PCF8574_1_ADDRESS, 0);
  bool switch2Pressed = digitalReadPCF(PCF8574_2_ADDRESS, 0);

  // Control the LEDs based on switch states
  digitalWritePCF(PCF8574_1_ADDRESS, 1, !switch1Pressed);  // Turn off LED1 if switch1 is pressed
  digitalWritePCF(PCF8574_2_ADDRESS, 1, switch2Pressed);  // Turn off LED2 if switch2 is pressed

  delay(500); // Blink every 500 ms
}

bool digitalReadPCF(uint8_t pcfAddress, int pin) {
  Wire.requestFrom(pcfAddress, 1);
  byte currentState = Wire.read();
  return bitRead(currentState, pin);
}

void digitalWritePCF(uint8_t pcfAddress, int pin, int value) {
  Wire.beginTransmission(pcfAddress);
  byte currentState = Wire.read();
  bitWrite(currentState, pin, value);
  Wire.write(currentState);
  Wire.endTransmission();
}
