int d1=D1;  //Digital Output
int d2=D2;  //Digital Input
void setup()
{
   pinMode(d1, OUTPUT);
   pinMode(d2, INPUT);
}

void loop()
{
  digitalWrite(d1,digitalRead(d2));
  delay(100);
}


