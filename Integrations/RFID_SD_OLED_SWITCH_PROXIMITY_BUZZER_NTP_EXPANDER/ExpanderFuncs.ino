
//Initialize I2C pins of the expander
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

//Writes one byte of data to expander pin 
//Params: Address of the , number of expander pin to write to, and wanted value to write
void digitalWritePCF(uint8_t pcfAddress, bool value) {
  // Read the state of proximity switch for PCF8574 with address pcfAddress
  bool switch1Pressed = digitalReadPCF(pcfAddress, 0);
  // Control the LEDs based on switch states
  // digitalWritePCF(pcfAddress, 1, value);  // Write value to pin 1 of pcfAddress
  Wire.beginTransmission(pcfAddress);
  byte currentState = Wire.read();
  bitWrite(currentState, 1, value);
  Wire.write(currentState);
  Wire.endTransmission();
  delay(500); // Blink every 500 ms
}

//Reads one byte of data from expander pin 
//Params: Address of the expander and number of expander pin to read from
bool digitalReadPCF(uint8_t pcfAddress, int pin) {
  Wire.requestFrom(pcfAddress, 1);
  byte currentState = Wire.read();
  return bitRead(currentState, pin);
}

