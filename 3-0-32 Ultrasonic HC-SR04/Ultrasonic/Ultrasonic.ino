//출력핀(trig)과 입력핀(echo) 연결 설정, 다른 핀을 연결해도 됨.
int trigPin = D1;
int echoPin = D2;
float distance,duration;
 
void setup(){
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
 
//초음파를 보낸다. 다 보내면 echo가 HIGH(신호받기) 상태로 대기
void loop(){
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);
 
  //trigPin 에서 LOW 신호를 보내고 echoPin으로 돌아 오는 시간을 축정
  //echoPin은 HIGH에서 LOW로 떨어지는 시간 측정
  duration = pulseIn(echoPin, HIGH);
  distance = (float)duration*0.017;
 
  //시리얼모니터
  Serial.print("Duration:");
  Serial.print(duration);
  Serial.print("       DIstance:");
  Serial.print(distance);
  Serial.println("cm\n");
  delay(500);
}

