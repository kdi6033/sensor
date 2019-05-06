int S = A0; // Analog Input
void setup() 
{
 Serial.begin(115200); // 시리얼 통신 초기화
 pinMode(S, INPUT);
}

void loop()
{
 Serial.println(analogRead(S));
 Serial.println(GetTemper(analogRead(S))/10);
 delay(100);
}

int GetTemper(int v)
{
 // 온도는 1차함수 온도 측정후 수정하세요
 // 10.0 도 일때 adc 값 300
 // 20.0 도 일때 adc 값 250
 return ( v-300 )* ((200 - 100) / (250 - 300)) + 100;
}

