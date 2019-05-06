int S = A0; // Analog Input
int d2=D2;  //Digital Input
void setup()
{
 Serial.begin(115200); // 시리얼 통신 초기화
 pinMode(S, INPUT);
 pinMode(d2, INPUT);
}

void loop()
{
 Serial.println(analogRead(S));
 Serial.println(digitalRead(d2));
 delay(200);
}


