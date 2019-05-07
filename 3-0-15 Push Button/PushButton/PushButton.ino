int d2=D2;  //Digital Input
int d4=D4;  //Digital Output
void setup()
{
 pinMode(d2, INPUT);
 pinMode(d4, OUTPUT);
}

void loop()
{
 digitalWrite(d4,digitalRead(d2));
 delay(100);
}


