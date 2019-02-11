int gpio13Led = 13;
int gpio12Relay = 12;
void setup() {
  // put your setup code here, to run once:
  pinMode(gpio13Led, OUTPUT);
  pinMode(gpio12Relay, OUTPUT);
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  Serial.println("test");
  digitalWrite(gpio13Led, HIGH);
  digitalWrite(gpio12Relay, HIGH);
  delay(2000);
  digitalWrite(gpio13Led, LOW);
  digitalWrite(gpio12Relay, LOW);;
}
