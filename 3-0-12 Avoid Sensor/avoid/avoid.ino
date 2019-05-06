int inD=D2;
void setup() {
  Serial.begin(115200);
  pinMode(inD,INPUT);
}

void loop() {
  Serial.println(digitalRead(inD));
  delay(500);
}

