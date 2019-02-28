const byte interruptPin = 2;
volatile int interruptCounter = 0;
unsigned long currentTime;
unsigned long cloopTime;
unsigned long nTotalFlow;

int Calc;  
 
void setup() {
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
}
 
void handleInterrupt() {
  interruptCounter++;
}
 
void loop() {
  currentTime = millis();
  if((currentTime >= (cloopTime + 1000)) && interruptCounter>0) {
      cloopTime = currentTime; // Updates cloopTime
      Calc = (interruptCounter * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour 
      nTotalFlow += Calc;
      interruptCounter=0;
      Serial.print(nTotalFlow);
      //Serial.print (Calc);
      Serial.print (" L/hour\r\n"); //Prints "L/hour" and returns a  new line
  }
 
}
