// 참조 사이트 https://www.best-microcontroller-projects.com/pcf8591.html
#include <Adafruit_PCF8591.h>
// Make sure that this is set to the value in volts of VCC
#define ADC_REFERENCE_VOLTAGE 5.0
Adafruit_PCF8591 pcf = Adafruit_PCF8591();

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("# Adafruit PCF8591 demo");
  if (!pcf.begin()) {
    Serial.println("# Adafruit PCF8591 not found!");
    while (1)
      delay(10);
  }
  Serial.println("# Adafruit PCF8591 found");
  pcf.enableDAC(true);

  Serial.println("AIN0, AIN1, AIN2, AIN3");
}

uint8_t dac_counter = 0;

void loop() {
  // Make a triangle wave on the DAC output
  Serial.print(dac_counter);
  Serial.print("  ");
  Serial.print((float)((float)dac_counter/50.8)); 
  Serial.print(":  ");
  pcf.analogWrite(dac_counter++); 

  Serial.print(int_to_volts(pcf.analogRead(0), 8, ADC_REFERENCE_VOLTAGE));
  Serial.print("V, ");
  Serial.print(int_to_volts(pcf.analogRead(1), 8, ADC_REFERENCE_VOLTAGE));
  Serial.print("V, ");
  Serial.print(int_to_volts(pcf.analogRead(2), 8, ADC_REFERENCE_VOLTAGE));
  Serial.print("V, ");
  Serial.print(int_to_volts(pcf.analogRead(3), 8, ADC_REFERENCE_VOLTAGE));
  Serial.print("V");
  Serial.println("");
  delay(1000);
}

float int_to_volts(uint16_t dac_value, uint8_t bits, float logic_level) {
  return (((float)dac_value / ((1 << bits) - 1)) * logic_level);
}
