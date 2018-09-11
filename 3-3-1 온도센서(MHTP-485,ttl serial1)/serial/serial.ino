String commandString = ":80040004000276";
const int contPin =  0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  serialEvent();
  requestTX();
  delay(2000);
}

void serialEvent() {
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    String inputString="";
    for(int i=0;i<len;i++)
      inputString += char(sbuf[i]);
    Serial.println(inputString);
    if( inputString.startsWith(":800404") == true) {
      int Humi, Temp;
      Serial.println("OK");
      Humi = HexToInt(sbuf[7])*16*16*16 + HexToInt(sbuf[8])*16*16 + 
      HexToInt(sbuf[9])*16 + HexToInt(sbuf[10]);
      Serial.println((float)Humi/100);
      Temp = HexToInt(sbuf[11])*16*16*16 + HexToInt(sbuf[12])*16*16 + 
      HexToInt(sbuf[13])*16 + HexToInt(sbuf[14]);
      Serial.println((float)Temp/100);
    }
  }
}

void requestTX() {
  Serial1.println(commandString);
}

int HexToInt(char hex) {
  if( hex <= '9')
    return hex - '0';
  else if (hex <= 'F')
    return hex-'A' +10;
  else if (hex <= 'f')
    return hex-'a' +10;
}

