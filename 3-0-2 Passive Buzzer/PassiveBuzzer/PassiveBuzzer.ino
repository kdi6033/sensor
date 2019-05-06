int d2=D2;
void setup()
{
 pinMode(d2, OUTPUT);
}

void loop()
{
  tone(d2,261.6); //도 출력
  delay(200);
  tone(d2,311.1); //레 출력
  delay(200);
  tone(d2,329.6); //미 출력
  delay(200);
  tone(d2,349.2); //파 출력
  delay(200);
  noTone(d2);
  delay(500);
}


