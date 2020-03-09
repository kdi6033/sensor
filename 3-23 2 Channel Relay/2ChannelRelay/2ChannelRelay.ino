void setup() {
  //Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.begin(115200);
  Serial.println("start");
}

void loop() {
  //turn relay 1 on
        Serial.write(0xA0);
        Serial.write(0x01);
        Serial.write(0x01);
        Serial.write(0xA2);
        delay(2000);
        //turn relay 1 off
        Serial.write(0xA0);
        Serial.write(0x01);
        Serial.write(0x00);
        Serial.write(0xA1);
        delay(2000);

        //turn relay 2 on
        Serial.write(0xA0);
        Serial.write(0x02);
        Serial.write(0x01);
        Serial.write(0xA3);
        delay(2000);
        //turn relay 2 off
        Serial.write(0xA0);
        Serial.write(0x02);
        Serial.write(0x00);
        Serial.write(0xA2);
        delay(2000);
}
