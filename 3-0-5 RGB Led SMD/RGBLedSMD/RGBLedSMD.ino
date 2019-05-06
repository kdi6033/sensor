int d1=D1;
int d2=D2;
int d3=D3;
void setup()
{
 pinMode(d1, OUTPUT);
 pinMode(d2, OUTPUT);
 pinMode(d3, OUTPUT);
}

void loop()
{
  digitalWrite(d1,255);  //Blue
  digitalWrite(d2,0);   //Green
  digitalWrite(d3,0);   //Red
  delay(1000);           
  digitalWrite(d1,0);  
  digitalWrite(d2,255);  
  digitalWrite(d3,0);   
  delay(1000);
  digitalWrite(d1,0);  
  digitalWrite(d2,0);  
  digitalWrite(d3,255); 
  delay(1000);
  digitalWrite(d1,255);  
  digitalWrite(d2,255);  
  digitalWrite(d3,255); 
  delay(1000);
}


