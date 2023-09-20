
void NTPsetup()
{
  // set notification call-back function
  //sntp_set_time_sync_notification_cb( timeavailable );
  /**
   * NTP server address could be aquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE aquired NTP server address
   */
  //sntp_servermode_dhcp(1);    // (optional)
  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagicaly.
   */
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  configTime(3 * 3600, 0, "time.google.com"); // Set time zone offset for Israel (2 hours ahead of UTC)


  /**
   * A more convenient approach to handle TimeZones with daylightOffset 
   * would be to specify a environmnet variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  //configTzTime(time_zone, ntpServer1, ntpServer2);

  //connect to WiFi
  Serial.printf("Connecting to %s ", wifi_ssid);
  IPAddress dns(8,8,8,8);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  int start_time = millis();
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

char* GetTimeString(){
  struct tm timeinfo;
  while(!getLocalTime(&timeinfo))
  {
    Serial.println("No time available (yet)");
    delay(1000);
  }
  char* time_string = (char*)malloc(100);
  strftime(time_string, 100, "%A, %B %d %Y %H:%M:%S", &timeinfo);
  return time_string;
}