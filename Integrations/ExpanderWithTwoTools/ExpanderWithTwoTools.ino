#include <Wire.h>
#include "driver/i2c.h"

#define PCF8574_1_ADDRESS 0x20
#define PCF8574_2_ADDRESS 0x27

// Define the SDA and SCL pins
#define I2C_SDA_PIN 26
#define I2C_SCL_PIN 27

void setup() {
  Wire.begin();
  i2c_init();
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

void i2c_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_SCL_PIN;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000; // 100 kHz

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
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
