//https://blog.squix.org/2016/07/esp8266-turn-a-9-body-scale-into-a-smart-scale-part-1.html
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D2;
const int LOADCELL_SCK_PIN = D3;

HX711 scale;

void setup() {
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {

  if (scale.is_ready()) {
    long reading = scale.read();
    Serial.print("HX711 reading: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711 not found.");
  }

  delay(1000);
  
}
