int S = A0; // INPUT PIN
void setup()
{
 Serial.begin(115200); // 시리얼 통신 초기화
 pinMode(S, INPUT);
}

void loop()
{
 Serial.println(analogRead(S));
 delay(100);
}


