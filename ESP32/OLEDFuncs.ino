
void InitializeOLED()
{
  SetLedColor(BLUE);
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE); // or BLACK;
  display.setTextSize(TEXT_SIZE);
  display.setCursor(0,0); 
  display.println(" Welcome,\n Please pass tag");
  display.display();
  UpdateTools();
}

