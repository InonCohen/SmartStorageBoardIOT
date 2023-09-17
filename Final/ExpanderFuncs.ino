
//Initialize I2C pins of the expander
// void i2c_init() {
//     // i2c_config_t conf;
//     // conf.mode = I2C_MODE_SLAVE;
//     // conf.sda_io_num = I2C_SDA_PIN;
//     // conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
//     // conf.scl_io_num = I2C_SCL_PIN;
//     // conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
//     // // conf.master.clk_speed = 400000; // 100 kHz
//     // i2c_param_config(I2C_NUM_0, &conf);
//     // i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
//         i2c_config_t i2c_config = {
//         .mode = I2C_MODE_SLAVE,
//         .sda_io_num = 26, // GPIO 26
//         .scl_io_num = 27, // GPIO 27
//         .sda_pullup_en = GPIO_PULLUP_ENABLE,
//         .scl_pullup_en = GPIO_PULLUP_ENABLE,
//         .slave.addr_10bit_en = 0,
//         .slave.slave_addr = I2C_SLAVE_ADDRESS
//     };

//     // Configure I2C controller with the given settings
//     i2c_param_config(I2C_NUM_0, &i2c_config);
//     i2c_driver_install(I2C_NUM_0, I2C_MODE_SLAVE, 0, 0, 0);
// }

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

void InitializeTools(){
  Wire.begin();
  //i2c_init();
  delay(1000);
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
