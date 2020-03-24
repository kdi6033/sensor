#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include <Q2HX711.h>

const byte hx711_data_pin = 12;
const byte hx711_clock_pin = 13; //SCK
Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISnO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND  923E6

unsigned int counter = 0;
long reading;

SSD1306 display(0x3c, 21, 22);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Sender Test");
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(923E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
  //LoRa.receive();
  Serial.println("init ok");
}

void loop() {
  //reading=(hx711.read()-16694000)/(-5000)*100;
  reading=hx711.read();
  Serial.println(reading);
  // send packet
  LoRa.beginPacket();
  LoRa.print("{\"weight\":");
  LoRa.print(reading);
  LoRa.print("}");
  LoRa.endPacket();
  counter++;
  delay(2000);                      
}
