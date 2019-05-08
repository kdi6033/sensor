int d2=D2;  //Digital Input
void setup()
{
 Serial.begin(115200); // 시리얼 통신 초기화
 pinMode(d2, INPUT);
}

void loop()
{
 Serial.println(digitalRead(d2));
 delay(200);
}


