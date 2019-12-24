//Step Motor OK42STH45-1684AYT  7TPSM4220
int DIR = D0;
int STP = D1;
int EN = D2;
void setup() {
  Serial.begin(9600);
  pinMode(DIR, OUTPUT); 
  pinMode(STP, OUTPUT);
  pinMode(EN, OUTPUT);
  
  digitalWrite(DIR, 1);
  digitalWrite(EN, 0);
  digitalWrite(STP, 0);
}

void rotate(int dir, int rev) {
  digitalWrite(DIR, dir);
  for(int j=0;j<rev;j++) {
    digitalWrite(EN, 1);
    for(int i=0;i<200;i++) {
      digitalWrite(STP, 1);
      delayMicroseconds(700);
      digitalWrite(STP, 0);
      delayMicroseconds(700);
    }
    digitalWrite(EN, 0);
    delay(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("start");
  rotate(1,10);
  delay(100);
  rotate(0,10);
  delay(100);
  
}
