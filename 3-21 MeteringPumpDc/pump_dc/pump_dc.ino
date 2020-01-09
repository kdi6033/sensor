
#include <ESP8266WiFi.h>

const int led1Pin =  4;
boolean bLED1 = false; 
const int led2Pin =  0;
boolean bLED2 = false; 

void setup() {
  // put your setup code here, to run once:
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);  
  
  Serial.begin(9600);
  delay(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("start");
  digitalWrite(led2Pin, LOW);
  delay(1500);
  digitalWrite(led2Pin, HIGH);
  delay(3000);
}
