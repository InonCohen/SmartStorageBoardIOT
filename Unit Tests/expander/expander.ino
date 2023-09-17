#include <Wire.h> // I2C Arduino Library

#define PCF8574_ADDR (0x20) // I2C Slave Address

 

void setup()

{

  Wire.begin();

}

 

void loop()

{


  Wire.beginTransmission(PCF8574_ADDR);

    Wire.write(0xAA); // see


    Wire.endTransmission();

    delay(1000);


  Wire.beginTransmission(PCF8574_ADDR);

    Wire.write(0x55); // see


    Wire.endTransmission();

    delay(1000);

}