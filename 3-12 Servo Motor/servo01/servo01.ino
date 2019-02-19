#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 

void setup() 
{ 
  myservo.attach(2);  // attaches the servo on GIO2 to the servo object 
} 
 
void loop() 
{ 
  int pos=90;
  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
} 

