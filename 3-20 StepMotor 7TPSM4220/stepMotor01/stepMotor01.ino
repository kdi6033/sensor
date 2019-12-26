//Step Motor OK42STH45-1684AYT  7TPSM4220
// 1 pulse  1.8도/indexer 만큼 회전
//indexer 1=200 pulse, 2=400 pulse, 4=800 pulse, 8=1600 pulse, 
// 16=3200 pulse,32=6400 pulse 를 조내면 motor는 1회 합니다.
int DIR = D0; // 회전방향
int STP = D1; // pulse
int EN = D2;  // step 진행 on off
void setup() {
  Serial.begin(9600);
  pinMode(DIR, OUTPUT); 
  pinMode(STP, OUTPUT);
  pinMode(EN, OUTPUT);
  
  digitalWrite(DIR, 0);
  digitalWrite(EN, 1);
  digitalWrite(STP, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("start");
  for(int i=0;i<2000;i++) {
      digitalWrite(STP, 1);
      delayMicroseconds(700);
      digitalWrite(STP, 0);
      delayMicroseconds(700);
  }
  delay(2000);
  
}
