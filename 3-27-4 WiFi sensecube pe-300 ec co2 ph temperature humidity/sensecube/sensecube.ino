// 다음 메뉴얼 참조하세요.
// crc16 rtu를 사용하기 위해 다음 라이브러리 설치하세요.
// crc16 modbus
//  https://github.com/RobTillaart/CRC
//  AUTHOR: Rob Tillaart
// 연산 사이트 https://crccalc.com/

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <arduino-timer.h>
#include "CRC.h"
#include <Ticker.h>
Ticker ticker;

// type 정의 pe300=1 co2=2 te&hu=3
int type=1;

#define TRIGGER_PIN 0 // trigger pin 0(D3) 2(D4)
ESP8266WebServer server(80);

const char* mqtt_server = ""; //브로커 주소
const char* outTopic = "/kdi/outTopic"; // 이름이 중복되지 않게 설정 기록
const char* inTopic = "/kdi/inTopic"; // 이름이 중복되지 않게 설정 기록
const char* clientName = "";  // setup 함수에서 자동생성
String sChipID;
char cChipID[20];
char Mqtt_Server[20]={0};

int count=0;
String inputString = "";         // 받은 문자열
char msg[100];
int i=0;

char str[24]={};
float val[5]={0.}; //센서의 계측된 값
String sIp=""; //접속된 ip의 주소

//json을 위한 설정
StaticJsonDocument<200> doc;
DeserializationError error;
JsonObject root;

WiFiClient espClient;
PubSubClient client(espClient);

void tick()
{
  if(!client.connected())
    return;
  crd16Rtu();
  //count++;
  //snprintf (msg, 75, "count #%d", count);
}

void setup() {
  Serial.begin(38400);
  Serial1.begin(38400);

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
 
  // AP 이름 자동으로 만듬 i2r-chipid
  sChipID = "i2r-"+String(ESP.getChipId(),HEX);
  sChipID.toCharArray(cChipID,sChipID.length()+1);
  clientName=&cChipID[0];
  Serial.println(sChipID);

  //wifi manager 참조유튜브  https://youtu.be/mJFd7g4jedw  //https://youtu.be/EzEG_2HcOQo
  //reset settings - wipe credentials for testing
  //wm.resetSettings();
  if (!wm.autoConnect(cChipID)) {
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
  //Serial.println("=============================");
  //Serial.println(mqtt_server);

  ticker.attach(5, tick);  //0.1 초도 가능
  //ticker.detach();

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

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
  }
  Serial.println();

  String s;
  deserializeJson(doc,payload);
  root = doc.as<JsonObject>();
  const char* sChipIDin = root["chipid"];
  if( sChipID.equals(sChipIDin)) {
    int onValue = root["on"];
    Serial.println(onValue);
    if( onValue==1) 
      digitalWrite(D4, 0);
    else
      digitalWrite(D4, 1);
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
    Serial.print(inChar,HEX);
    Serial.print(" ");
    // add it to the inputString:
    inputString += inChar;
  }

  int rs;
  int cLength;
  if(type==1)
    cLength=11;
  else if(type==2)
    cLength=9;
  else if(type==3)
    cLength=9;
    
  if(inputString.length() >= cLength) {
    Serial.println("");
    uint16_t myVar = 0;

    for(int i=0; i<str[5]; i++) {
      myVar=inputString.charAt(i*2+3);
      myVar <<= 8;
      myVar = myVar | inputString.charAt(i*2+4);
      rs=String(myVar,DEC).toInt();
      val[i]=(float)rs;
    }

    // 계측된 센서 소숫점 자리수 보정
    if(type==1) {
      val[0]/=10;
      val[1]/=100;
      val[2]/=10;
    }
    else if(type==2) {
      val[0]/=1;
    }
    else if(type==3) {
      val[0]/=10;
      val[1]/=10;
    }
    mqtt_send();
    inputString="";
    Serial.println("");
  }

}

void mqtt_send() {
  char cIp[20]={0};
  sIp.toCharArray(cIp, sIp.length()+1);
  if(type==1)
    snprintf (msg, 100, "{\"chip\":\"%s\", \"type\":1, \"ec\":%.1f, \"ph\":%.1f, \"te\":%.1f, \"ip\":\"%s\"}", cChipID,val[0],val[1],val[2],cIp);
  else if(type==2)
    snprintf (msg, 100, "{\"chip\":\"%s\", \"type\":2, \"co2\":%.1f, \"ip\":\"%s\"}", cChipID,val[0],cIp);
  else if(type==3)
    snprintf (msg, 100, "{\"chip\":\"%s\", \"type\":3, \"hu\":%.1f, \"te\":%.1f, \"ip\":\"%s\"}", cChipID,val[0],val[1],cIp);
  client.publish(outTopic, msg);
  Serial.println(msg);
}

// 아두이노에서 RS485 출력을 내보낸다.
void crd16Rtu() {
  String s;
  int si,sj,len;

  if(type==1) {
    str[0]=0x1f;str[1]=0x04;str[2]=0x00;str[3]=0x01;str[4]=0x00;str[5]=0x03; //pe300
  }
  else if(type==2) {
    str[0]=0x1f;str[1]=0x04;str[2]=0x00;str[3]=0x04;str[4]=0x00;str[5]=0x01; //co2
  }
  else if(type==3) {
    str[0]=0x1f;str[1]=0x04;str[2]=0x00;str[3]=0x40;str[4]=0x00;str[5]=0x02; // Te Hu
  }
  //char str[24] =  {0x1f,0x04,0x00,0x01,0x00,0x03,0x00,0x00};  //pe300
  //char str[24] =  {0x1f,0x04,0x00,0x04,0x00,0x01,0x00,0x00};  //co2
  //char str[24] =  {0x1f,0x04,0x00,0x40,0x00,0x02,0x00,0x00}; // Te Hu 115200
  
  len=6;
  
  uint8_t * data = (uint8_t *) &str[0];
  si=crc16(data, len, 0x8005, 0xFFFF, 0x0000, true,  true  );
  sj=si&0xff;
  str[len]=sj;
  sj=si>>8;
  str[len+1]=sj;

  for(int i=0;i<len+2;i++) {
    Serial.print(str[i],HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
  for(int i=0;i<len+2;i++)
    Serial1.print(str[i]);
}
