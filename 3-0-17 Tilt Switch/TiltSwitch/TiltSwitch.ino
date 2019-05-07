int d2=D2;  //Digital Input
int d4=D4;  //Digital Output
void setup()
{
  Serial.begin(115200); // 시리얼 통신 초기화
  pinMode(d2, INPUT);
  pinMode(d4, OUTPUT);
}

void loop()
{
  Serial.println(digitalRead(d2));
  digitalWrite(d4,digitalRead(d2));
  delay(200);
}


