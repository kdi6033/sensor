// 참조 사이트 https://www.best-microcontroller-projects.com/pcf8591.html
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_PCF8591.h>
#include <Ticker.h>
Ticker ticker;
#define ADC_REFERENCE_VOLTAGE 5.0
Adafruit_PCF8591 pcf = Adafruit_PCF8591();
uint8_t dac_counter = 0;
ESP8266WebServer server(80);

char mac[20];  //mac address
String sMac;
String inputString = "";         // 받은 문자열
int valueSet;
float a0,a1,a2,a3;

void tick()
{
  a0=int_to_volts(pcf.analogRead(0), 8, ADC_REFERENCE_VOLTAGE);
  a1=int_to_volts(pcf.analogRead(1), 8, ADC_REFERENCE_VOLTAGE);
  a2=int_to_volts(pcf.analogRead(2), 8, ADC_REFERENCE_VOLTAGE);
  a3=int_to_volts(pcf.analogRead(3), 8, ADC_REFERENCE_VOLTAGE);
  // Make a triangle wave on the DAC output
  //Serial.print(int_to_volts(pcf.analogRead(0), 8, ADC_REFERENCE_VOLTAGE));
  //Serial.print("V, ");
  //Serial.print(int_to_volts(pcf.analogRead(1), 8, ADC_REFERENCE_VOLTAGE));
  //Serial.print("V, ");
  //Serial.print(int_to_volts(pcf.analogRead(2), 8, ADC_REFERENCE_VOLTAGE));
  //Serial.print("V, ");
  //Serial.print(int_to_volts(pcf.analogRead(3), 8, ADC_REFERENCE_VOLTAGE));
  //Serial.print("V");
  //Serial.println("");
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
 
  //이름 자동으로 생성
  byte hardwareMAC[12];  //WIFI MAC address
  WiFi.macAddress(hardwareMAC);
  sprintf(mac,"%02x%02x%02x%02x%02x%02x",hardwareMAC[5], hardwareMAC[4], hardwareMAC[3], hardwareMAC[2], hardwareMAC[1], hardwareMAC[0]);  
  sMac=mac;
  
  //wifi manager 참조유튜브  https://youtu.be/mJFd7g4jedw  //https://youtu.be/EzEG_2HcOQo
  //reset settings - wipe credentials for testing
  //wm.resetSettings();
  char i2rMac[30];
  sprintf(i2rMac,"i2r-%02x%02x%02x%02x%02x%02x",hardwareMAC[5], hardwareMAC[4], hardwareMAC[3], hardwareMAC[2], hardwareMAC[1], hardwareMAC[0]);  
  if (!wm.autoConnect(i2rMac)) {
    Serial.println("failed to connect and hit timeout");
    ESP.restart();
    delay(1000);
  }
  Serial.print("connected... :");
  Serial.print(WiFi.localIP().toString());

  ticker.attach(5, tick);  //0.1 초도 가능
  //ticker.detach();
  
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/on", handleOn);
  server.begin();
  Serial.println("HTTP server started");
  

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

void loop() {
  server.handleClient();
}

float int_to_volts(uint16_t dac_value, uint8_t bits, float logic_level) {
  return (((float)dac_value / ((1 << bits) - 1)) * logic_level);
}
