#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include "images.h"
#include <Q2HX711.h>

const byte hx711_data_pin = 12;
const byte hx711_clock_pin = 13;
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

//SSD1306 display(0x3c, 21, 22);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

//==================================================
//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16
SSD1306  display(0x3c, 4, 15);

void drawFontFaceDemo() {
    display.clear();
    String s="Send: ";
    s=s+counter;
    Serial.println(s);
    display.drawString(0, 0, s);
    s="w  ";
    s=s+String(reading);
    display.drawString(0, 20, s);
    display.display();
    display.display();
}

void setup() {
  DisplaySetup();
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

void DisplaySetup() {
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void loop() {
  //Serial.println(String(counter));
  drawFontFaceDemo();

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
  delay(1000);         
}
