#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* Set these to your desired credentials. */
char ssid[32] = "i2r";
char password[32] = "00000000";
ESP8266WebServer server(80);

/* AP 설정을 위한 변수 선언 */
String sAP_ssid,sChipID;
char cAP_ssid[20];
char cChipID[10]; // Json에서 chipid를 고유 ID로 사용

boolean connect;
const int led = 2;
int relay[2]={0,0};

// mqtt를 위한 설정
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* outTopic = "/test/outTopic"; // 이름이 중복되지 않게 설정 기록
const char* inTopic = "/test/inTopic"; // 이름이 중복되지 않게 설정 기록
const char* clientName = "";  // setup 함수에서 자동생성
int no=1; //기기번호

WiFiClient espClient;
PubSubClient client(espClient);

//json
StaticJsonDocument<300> doc;
DeserializationError error;
JsonObject root;

void setup(void) {
  Serial.begin(115200);
  
  // AP 이름 자동으로 만듬
  sChipID=String(ESP.getChipId(),HEX);
  sChipID.toCharArray(cChipID,sChipID.length()+1);
  clientName=&cChipID[0];

  connectWifi();

  server.on("/", handleRoot);
  server.on("/relay0", handleRelay0);
  server.on("/relay1", handleRelay1);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/scan", handleScan);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  // mqtt 설정
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void connectWifi() {
  Serial.println("STATION Mode");
  WiFi.disconnect();
  WiFi.mode(WIFI_STA); 
  //IPAddress ip(192, 168, 0, 201); 
  //IPAddress gateway(192, 168, 0, 1);
  //IPAddress subnet(255, 255, 255, 0);
  //WiFi.config(ip, gateway, subnet); // before or after Wifi.Begin(ssid, password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // 상태 led로 표시
  for(int i=0;i<10;i++) {
    Serial.println("STATION MOde Connected ");
    delay(500);
  }
}

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  deserializeJson(doc,payload);
  root = doc.as<JsonObject>();
  const char* rChip = root["chip"];
  relay[0] = root["relay0"];
  relay[1] = root["relay1"];
  Serial.println(relay[0]);
  Serial.println(relay[1]);
  relayDo();
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

void loop(void) {
  if (!client.connected()) 
    reconnect();
  client.loop();

  server.handleClient();
}

void relayDo() {
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
  delay(500);
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
