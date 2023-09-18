
//Writes one byte of data to expander pin 
//Params: Address of the , number of expander pin to write to, and wanted value to write
// void digitalWritePCF(uint8_t pcfAddress, bool value) {
//   // Read the state of proximity switch for PCF8574 with address pcfAddress
//   bool switch1Pressed = digitalReadPCF(pcfAddress, 0);
//   // Control the LEDs based on switch states
//   // digitalWritePCF(pcfAddress, 1, value);  // Write value to pin 1 of pcfAddress
//   Wire.beginTransmission(pcfAddress);
//   byte currentState = Wire.read();
//   bitWrite(currentState, 1, value);
//   Wire.write(currentState);
//   Wire.endTransmission();
//   delay(500); // Blink every 500 ms
// }

//Reads one byte of data from expander pin 
//Params: Address of the expander and number of expander pin to read from
// bool digitalReadPCF(uint8_t pcfAddress, int pin) {
//   Wire.requestFrom(pcfAddress, 1);
//   byte currentState = Wire.read();
//   return bitRead(currentState, pin);
// }

void InitializeTools(){
  // Wire1.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
  // Serial.print("The Wire function called result is ");
  // Serial.println(res);
  pcf1.begin(PCF8574_1_ADDRESS, &Wire1);
  // delay(1000);
  pcf2.begin(PCF8574_2_ADDRESS, &Wire1);
  delay(1000);
  for (int i = 0; i < 3; i++) {
    pcf1.pinMode((2 * i) + 1, INPUT);
    pcf1.pinMode(2 * i, OUTPUT);
    pcf2.pinMode((2 * i) + 1, INPUT);
    pcf2.pinMode(2 * i, OUTPUT);
  }
}
