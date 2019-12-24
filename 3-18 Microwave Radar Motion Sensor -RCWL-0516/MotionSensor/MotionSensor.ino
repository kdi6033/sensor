void setup() {
  pinMode(D3,INPUT); // RCWL-1506 OUT핀
  pinMode(D4,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int val = digitalRead(D3);
  Serial.println(val);
  digitalWrite(D4,val);
  delay(500);
}
