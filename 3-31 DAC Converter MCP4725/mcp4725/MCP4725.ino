#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

void setup(void) {
  Serial.begin(9600);
  dac.begin(0x60);
}

void loop(void) {
  Serial.println("4095");
  dac.setVoltage(4095, false);
  delay(3000);
  Serial.println("1");
  dac.setVoltage(1, false);
  delay(3000);
}
