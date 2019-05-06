int d1=D1;
int d2=D2;
void setup()
{
 pinMode(d1, OUTPUT);
 pinMode(d2, OUTPUT);
}

void loop()
{
  digitalWrite(d1,255);  //Red
  digitalWrite(d2,0);  //Yellow
  delay(1000);           
  digitalWrite(d1,0);
  digitalWrite(d2,255);
  delay(1000);
  digitalWrite(d1,40);
  digitalWrite(d2,128);
  delay(1000);
}


