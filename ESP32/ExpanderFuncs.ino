
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
