#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <arduino-timer.h>
#include <Ticker.h>
Ticker ticker;

#define TRIGGER_PIN 0 // trigger pin 0(D3) 2(D4)
ESP8266WebServer server(80);

//const char* mqtt_server = "broker.mqtt-dashboard.com"; //브로커 주소
const char* mqtt_server = ""; //브로커 주소
const char* outTopic = "/kdi/outTopic"; // 이름이 중복되지 않게 설정 기록
const char* inTopic = "/kdi/inTopic"; // 이름이 중복되지 않게 설정 기록
const char* clientName = "";  // setup 함수에서 자동생성
char mac[20];  //mac address
String sMac;
char Mqtt_Server[20]={0};

int count=0;
String inputString = "";         // 받은 문자열
char msg[100];
int i=0;

//char str[24]={};
String sIp=""; //접속된 ip의 주소

//json을 위한 설정
StaticJsonDocument<200> doc;
DeserializationError error;
JsonObject root;

// sonoff variable
int gpio13Led = 13;
int gpio12Relay = 12;
int relayPower=0;

WiFiClient espClient;
PubSubClient client(espClient);

void tick()
{
  if(!client.connected())
    return;
  count++;
  /*
  snprintf (msg, 75, "count #%d", count);
  client.publish(outTopic, msg);
  Serial.println(msg);
  */
}

void setup() {
  // preparing sonoff GPIOs
  pinMode(gpio13Led, OUTPUT);
  digitalWrite(gpio13Led, HIGH);
  pinMode(gpio12Relay, OUTPUT);
  digitalWrite(gpio12Relay, HIGH);
  //pinMode(D4, OUTPUT);
  //digitalWrite(D4, HIGH);
  
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
 
  //이름 자동으로 생성
  byte hardwareMAC[12];  //WIFI MAC address
  WiFi.macAddress(hardwareMAC);
  sprintf(mac,"%02x%02x%02x%02x%02x%02x",hardwareMAC[5], hardwareMAC[4], hardwareMAC[3], hardwareMAC[2], hardwareMAC[1], hardwareMAC[0]);  
  clientName=&mac[0];
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
  // mqtt_server =*.*.*.200으로 자동 설정
  sIp=WiFi.localIP().toString();
  String MqttServerIp=sIp.substring(0,sIp.lastIndexOf(".")+1)+"200";
  MqttServerIp.toCharArray(Mqtt_Server, MqttServerIp.length()+1);
  mqtt_server=&Mqtt_Server[0];

  ticker.attach(5, tick);  //0.1 초도 가능
  //ticker.detach();

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println(clientName);
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

  String s;
  deserializeJson(doc,payload);
  root = doc.as<JsonObject>();
  const char* mac = root["mac"];
  if( sMac.equals(mac)) {
    int onValue = root["on"];
    Serial.println(onValue);
    if( onValue==1)
      lightOnOff(1);
    else
      lightOnOff(0);
  }
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

void loop() {
  server.handleClient();
  serialEvent();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void serialEvent() {
  if(Serial.available() == false)
    return;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
  }
}

void lightOnOff(int valueOn) {
  if( valueOn == 1) {
    digitalWrite(gpio13Led, LOW);
    digitalWrite(gpio12Relay, HIGH);
    //digitalWrite(D4, 0);
  }
  else {
    digitalWrite(gpio13Led, HIGH);
    digitalWrite(gpio12Relay, LOW);
    //digitalWrite(D4, 1);
  }
  delay(100);
}
