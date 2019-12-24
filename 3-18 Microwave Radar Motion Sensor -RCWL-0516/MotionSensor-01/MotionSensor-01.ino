// RCWL-0516
int inSensor=D3;
int outLed=D4;
int valLed=1;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 5000; 
  
void setup() {
  pinMode(inSensor,INPUT); // RCWL-1506 OUT핀
  pinMode(outLed,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // 움직임이 있으면 val=1;
  int val = digitalRead(inSensor);
  unsigned long currentMillis = millis();
  //Serial.println(val);
  //delay(500);
  if(val==1){
    digitalWrite(outLed,0);
    valLed=0;
  }
  else
    valLed=1;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(outLed,valLed);
  }
}
