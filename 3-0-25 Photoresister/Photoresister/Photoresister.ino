int S = A0; // Analog Input
void setup()
{
 Serial.begin(115200); // 시리얼 통신 초기화
 pinMode(S, INPUT);
}

void loop()
{
 Serial.println(analogRead(S));
 delay(200);
}


