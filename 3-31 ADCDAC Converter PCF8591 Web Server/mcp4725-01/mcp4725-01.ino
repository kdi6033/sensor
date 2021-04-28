#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello!");

  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
  // For MCP4725A0 the address is 0x60 or 0x61
  // For MCP4725A2 the address is 0x64 or 0x65
  dac.begin(0x60);
    
  Serial.println("Generating a triangle wave");
}

void loop(void) {
  Serial.println("4095");
  dac.setVoltage(4095, false);
  delay(3000);
  Serial.println("1");
  dac.setVoltage(1, false);
  delay(3000);

}
