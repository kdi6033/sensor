int totalCount = 0;
int oldA = LOW;
int oldB = LOW;
int S = D1;     // SW, Switch
int pinA = D2;  // DT, OutA
int pinB = D3;  // CLK,  OutB

void setup()
{
   Serial.begin(115200); // 시리얼 통신 초기화
   Serial.println("Rotary Encoders");
   pinMode(S, INPUT_PULLUP);
   pinMode(pinA, INPUT);
   pinMode(pinB, INPUT);
}

int nowCnt = 0;
void loop()
{
   if (LOW == digitalRead(S))  // 스위치 누르면 counter=0 으로 초기화
   {
     Serial.println("Clear"); 
     delay(200);
     totalCount = 0;
   }
   nowCnt = getValue();
   if (nowCnt != 0)
   {
     totalCount += nowCnt;
     Serial.println(totalCount);
   }
}

int getValue()
{
   int rValue = 0;
   int A = digitalRead(pinA);
   int B = digitalRead(pinB);
  
   if (A != oldA || B != oldB)
   {
     if (oldA == LOW && A == HIGH)
     {
       if (oldB == HIGH) rValue = 1;
       else rValue = -1;
     }
   }
   oldA = A;
   oldB = B;
   return rValue;
}



