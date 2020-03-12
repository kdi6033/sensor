#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define TRIGGER_PIN 0 // trigger pin 0(D3) 2(D4)
/* Set these to your desired credentials. */
const char *softAP_ssid = "i2r-";
const char *softAP_password = "";
char ssid[32] = "";
char password[32] = "";
/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
ESP8266WebServer server(80);

/* AP 설정을 위한 변수 선언 */
String sAP_ssid,sChipID;
char cAP_ssid[20];
char cChipID[10]; // Json에서 chipid를 고유 ID로 사용

boolean connect;
const int led = 2;
int bootMode=0; //0:station  1:AP
int relay[2];

void setup(void) {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  
  // AP 이름 자동으로 만듬
  sChipID=String(ESP.getChipId(),HEX);
  sChipID.toCharArray(cChipID,sChipID.length()+1);
  sAP_ssid=String(softAP_ssid)+String(ESP.getChipId(),HEX);
  sAP_ssid.toCharArray(cAP_ssid,sAP_ssid.length()+1);
  softAP_ssid=&cAP_ssid[0];
  //clientName=softAP_ssid; //mqtt 통신에서 사용

  loadCredentials();
  if(ssid[0]==0)
    setupAp();
  else
    connectWifi();

  server.on("/", handleRoot);
  server.on("/relay0", handleRelay0);
  server.on("/relay1", handleRelay1);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  server.on("/scan", handleScan);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void setupAp() {
  bootMode=1;
  Serial.println("AP Mode");
  Serial.println(softAP_ssid);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  digitalWrite(led, 0); //상태 led로 표시
}

void connectWifi() {
  bootMode=0;
  Serial.println("STATION Mode");
  WiFi.disconnect();
  WiFi.mode(WIFI_STA); 
  IPAddress ip(172, 30, 1, 201); 
  IPAddress gateway(172, 30, 1, 254);
  //IPAddress ip(192, 168, 0, 201); 
  //IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet); // before or after Wifi.Begin(ssid, password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // 상태 led로 표시
  for(int i=0;i<10;i++) {
    Serial.println("AP MOd AP MOd AP MOd AP MOd AP MOd AP MOd");
    delay(500);
  }
}

void loop(void) {
  server.handleClient();

  if(bootMode==1) {
    Serial.println("AP MOd AP MOd AP MOd AP MOd AP MOd AP MOd");
    delay(500);
  }

  if ( digitalRead(TRIGGER_PIN) == LOW ) 
    factoryDefault();
}

void relay0() {
  if(relay[0]==1) {
      //turn relay 1 on
      Serial.write(0xA0);
      Serial.write(0x01);
      Serial.write(0x01);
      Serial.write(0xA2);
  }
  else {
      //turn relay 1 off
      Serial.write(0xA0);
      Serial.write(0x01);
      Serial.write(0x00);
      Serial.write(0xA1);   
  }
}

void relay1() {
  if(relay[1]==1) {
      //turn relay 2 on
      Serial.write(0xA0);
      Serial.write(0x02);
      Serial.write(0x01);
      Serial.write(0xA3);
  }
  else {
      //turn relay 2 off
      Serial.write(0xA0);
      Serial.write(0x02);
      Serial.write(0x00);
      Serial.write(0xA2);   
  }
}
