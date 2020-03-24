#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include "images.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

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

char ssid[32] = "i2r";
char password[32] = "00000000";
unsigned long lastConnectTry = 0;
unsigned int sWifi;

// mqtt를 위한 설정
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* outTopic = "/ims1234/inTopic";
const char* inTopic = "/ims1234/outTopic";
const char* clientName = "aaa"; // AP 이름과 같이 사용
char msg[100];

WiFiClient espClient;
PubSubClient client(espClient);

int connectWifi(){
  //WiFi.disconnect();
  //WiFi.mode(WIFI_STA); //이 모드를 설정 않해야 AP가 살아있습니다.
  //IPAddress ip(192, 168, 0, 201); // this 3 lines for a fix IP-address
  //IPAddress gateway(192, 168, 0, 1);
  //IPAddress subnet(255, 255, 255, 0);
  //WiFi.config(ip, gateway, subnet); // before or after Wifi.Begin(ssid, password);
  WiFi.begin(ssid, password);
  delay(3000);

  //referance: https://www.arduino.cc/en/Reference/WiFiStatus
  //WL_NO_SHIELD:255 WL_IDLE_STATUS:0 WL_NO_SSID_AVAIL:1 WL_SCAN_COMPLETED:2
  //WL_CONNECTED:3 WL_CONNECT_FAILED:4 WL_CONNECTION_LOST:5 WL_DISCONNECTED:6
  //int connRes = WiFi.waitForConnectResult();
  sWifi = WiFi.status();
  Serial.print ( "wifi status: " );
  Serial.println ( sWifi );
  if(sWifi == WL_CONNECTED){
    Serial.println("well connected");
    Serial.println(WiFi.localIP());
  }
  else
    WiFi.disconnect();
  drawFontFaceDemo();
  return sWifi;
}

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
  client.publish(outTopic, msg);
}

void setup() {
  DisplaySetup();
  Serial.begin(115200);
  connectWifi();
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

  // mqtt 설정
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial1.print((char)payload[i]);
  }
  Serial.println();
}

// mqtt 통신에 지속적으로 접속한다.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientName)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Reconnected");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
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
  
  long now = millis();
  //6초에 한번 와이파이 끊기면 다시 연결
  sWifi = WiFi.status();
  //Serial.println(sWifi);
  if (sWifi == WL_CONNECTED ) {
    if (!client.connected() && sWifi==WL_CONNECTED) {
      reconnect();
    }
    client.loop();
  }
  else {
    if(now > (lastConnectTry + 6000)) {
      lastConnectTry=now;
      Serial.println ( "Connect requested" );
      connectWifi();
    }
  }
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
