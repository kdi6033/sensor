//https://aws.amazon.com/ko/blogs/compute/building-an-aws-iot-core-device-using-aws-serverless-and-an-esp32/
// youtube https://youtu.be/kgJMwPFN30M  AWS+ESP32 참조
#include <pgmspace.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include "images.h"
#include <time.h>

#define SECRET
#define THINGNAME "Mtest"
// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "outTopic"
#define AWS_IOT_SUBSCRIBE_TOPIC "inTopic"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

const char WIFI_SSID[] = "i2r";
const char WIFI_PASSWORD[] = "00000000";
const char AWS_IOT_ENDPOINT[] = "a2drjbpnpn5qah-ats.iot.us-east-2.amazonaws.com";

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    923E6

String rssi = "RSSI --";
String packSize = "--";
String packet ;

//==================================================
//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16
SSD1306  display(0x3c, 4, 15);

unsigned long lastConnectTry = 0;
unsigned int sWifi;
uint64_t chipid;
String chipidS;
char msg[100];

void drawFontFaceDemo() {
    display.clear();
    String s;
    if(sWifi!=WL_DISCONNECTED)
      s="ip "+toStringIp(WiFi.localIP());
    else
      s="wifi disconnected";
    display.drawString(0, 0, s);
    display.drawString(0, 20, packet);
    display.display();
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  Serial.println(rssi);
  Serial.println(packet);
  drawFontFaceDemo();
  packet.toCharArray(msg, packet.length()+1);
  client.publish(AWS_IOT_PUBLISH_TOPIC, msg);
}

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  drawFontFaceDemo();

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["sensor_a0"] = analogRead(0);
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

//  StaticJsonDocument<200> doc;
//  deserializeJson(doc, payload);
//  const char* message = doc["message"];
}

void setup() {
  DisplaySetup();
  Serial.begin(9600);
  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  chipidS=String(((uint16_t)(chipid>>32)),HEX)+String(((uint32_t)chipid),HEX);
  Serial.println(chipidS);
  connectAWS();

    while (!Serial);
  Serial.println(); 
  Serial.println("LoRa Receiver Callback");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(923E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
  LoRa.receive();
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
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize);  }
  delay(10);
  //publishMessage();
  client.loop();
}
