
int buzzer=5;

void setup() {

   pinMode(buzzer, OUTPUT);

}

 
void loop() {

     //turn on the buzzer at 1535 frequency for 500 milliseconds

     tone(buzzer,2000,700);
     //add another 500 milliseconds of silence

     tone(buzzer,0,200);

}