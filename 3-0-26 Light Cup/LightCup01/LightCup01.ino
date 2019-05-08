int d1=D1;  //Digital Output
int d2=D2;  //Digital Input
int bright=0;
void setup()
{
   pinMode(d1, OUTPUT);
   pinMode(d2, INPUT);
}

void loop()
{
  if(digitalRead(d2) == HIGH)
    bright +=1;
  else
    bright = 0;
  if(bright > 255)
    bright = 255;
  analogWrite(d1,bright);
  delay(50);
}


