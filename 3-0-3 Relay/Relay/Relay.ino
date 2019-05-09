int d2=D2;
void setup()
{
 Serial.begin(115200); // 시리얼 통신 초기화
 pinMode(d2, OUTPUT);
}

void loop()
{
  digitalWrite(d2,HIGH);
  delay(1000);
  digitalWrite(d2,LOW);
  delay(1000);
}


