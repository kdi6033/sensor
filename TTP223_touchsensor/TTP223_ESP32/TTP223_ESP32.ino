#define LED 2

int touchPin = 4;// connect output from TTP223 to this
int val = 0; 
//int relayPin = 8;// Connected to relay

void setup() {
 Serial.begin(9600);
 pinMode(touchPin, INPUT); 
  pinMode(LED, OUTPUT);

}

void loop() {
  digitalWrite(LED, LOW);
  val = digitalRead(touchPin); 
  if(val ==1){
  Serial.println("Touched");
  digitalWrite(LED, HIGH);
  }
  delay(100);
  //Serial.println();
}
