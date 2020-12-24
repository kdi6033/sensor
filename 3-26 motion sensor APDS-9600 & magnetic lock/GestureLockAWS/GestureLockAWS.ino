// 회로도 및 보드 구매 : 통신보드 구매사이트 : https://smartstore.naver.com/i2r/products/4584482642
// AWS ubuntu 서버만들기 : https://youtu.be/UvUXYkpm8Hw
// AWS 사물인터넷 IOT & 인증서 만들기 https://youtu.be/_y6z8kreg00
// AWS에 ESP8266 연결 : https://youtu.be/SIn917VvkmE
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <arduino-timer.h>
#include <ACROBOTIC_SSD1306.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <time.h>
#define TRIGGER_PIN D5 // trigger pin 0(D3) 2(D4)

// motion sensor
#define APDS9960_INT    D5 // Needs to be an interrupt pin
void ICACHE_RAM_ATTR interruptRoutine();
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

auto timer = timer_create_default(); // create a timer with default settings

String sChipId="";
char cChipId[40]="";
ESP8266WebServer server(80);

String inputString = "";         // 받은 문자열
String s;
int p4[8]={0};

int relay=D0;
const int In = D7;
char msg[50];

//mqtt 통신 변수
const char *thingId = "";          // 사물 이름 (thing ID) 
const char *host = "east-2.amazonaws.com"; // AWS IoT Core 주소
const char* outTopic = "outTopic"; // 사용자가 결정해서 기록
const char* inTopic = "inTopic"; // 사용자가 결정해서 기록

//json을 위한 설정
StaticJsonDocument<200> doc;
DeserializationError error;
JsonObject root;

// 사물 인증서 (파일 이름: xxxxxxxxxx-certificate.pem.crt)
const char cert_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVANDU3YnFvvVAlD2NaadrdwFcWTYyMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDExMTMwNTEw
oXQdZmamNV08yni2i+cEBsMob29fMyWvYggdWDgf+7Q5ZAppnj/aD5/dsWRVmgV/
x77xQzrbXXF8gOWW63/kQU2dU4LBec1a15dM78GnqUCPnhpSnyR1giMztbggEA==
-----END CERTIFICATE-----
)EOF";

// 사물 인증서 프라이빗 키 (파일 이름: xxxxxxxxxx-private.pem.key)
const char key_str[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAwctO1Mp/+ZWY58zzHpoURm7qzmaul8CPffKWODOQhDBFXYSR
okDvE+9RWbfsiVxexhBpG3qUGzUK6Mbap9jMRvYtVkOVDTFsBpAekMlvWrJTQDqa
VLkVuzhGnW/hAQ2tqbzSwz+3JCruAXLKVtCVaKNGx/UbLLWXd3tLFD/q9ubs+Oti
rUycTQKBgHrXNBQpCc/YWFQpD9yKHbNiw5pbGiiB6m2Bl6gYPWyUeGo+JjnqTw1J
4+MxYQzW5LUVokblwJw8d/bLbzwnHsdHUd/tVgNJkfEtiRxY35SMrC/aYzL/vIVU
hTbqf0SOmSWY9hkaEWAyxiaeSho3uz9kUwZCPMMziTgEEm1qoyIX
-----END RSA PRIVATE KEY-----
)EOF";

// Amazon Trust Services(ATS) 엔드포인트 CA 인증서 (서버인증 > "RSA 2048비트 키: Amazon Root CA 1" 다운로드)
const char ca_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

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

  if(!rChip)
    return;

  if(strcmp(rChip,cChipId)==0) {
    int onValue = root["on"];  
    if(onValue==1) {
      digitalWrite(relay,HIGH);
      p4[0]=1;
    }
    else if(onValue==0) {
      digitalWrite(relay,LOW);
      p4[0]=0;
    }
  }
}

X509List ca(ca_str);
X509List cert(cert_str);
PrivateKey key(key_str);
WiFiClientSecure wifiClient;
PubSubClient client(host, 8883, callback, wifiClient); //set  MQTT port number to 8883 as per //standard

// 일정시간 마다 동작하게 하려면 여기에 프로그램 하세요
bool toggle_led(void *) {

  return true; 
}

void setup() {
  // call the toggle_led function every 1000 millis (1 second)
  timer.every(5000, toggle_led);
  
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(APDS9960_INT, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(In, INPUT);

  //Oled Setup
  Wire.begin();  
  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen

  oled.setTextXY(2,4);     
  oled.putString("Set Wifi");
  
  // put your setup code here, to run once:

  // AP 이름 자동으로 만듬 i2r-chipid
  sChipId = "i2r-"+String(ESP.getChipId(),HEX);
  sChipId.toCharArray(cChipId,sChipId.length()+1);
  Serial.println(sChipId);

  //WiFiManager
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  // 전원 인가시 D7 핀을 GND에 연결하면 와이파이 초기 상태로 리셋된다.
  if(digitalRead(In)==0) {
    wm.resetSettings();
    ESP.restart();
  }
  // wm.resetSettings(); //reset settings - for testing

  if (!wm.autoConnect(cChipId)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected... :)");
  Serial.println(WiFi.localIP());
  String ss,s;
  ss=toStringIp(WiFi.localIP()); 
  s=ss.substring(0,8);  
  oled.setTextXY(2,4);     
  oled.putString(s);
  s=ss.substring(8,ss.length());  
  oled.setTextXY(4,4);     
  oled.putString(s);

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/scan", handleScan);
  server.on("/wifi", handleWifi);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  // Initialize interrupt service routine
  //attachInterrupt(0, interruptRoutine, FALLING);
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
  //attachInterrupt(digitalPinToInterrupt(APDS9960_INT), ISR, mode);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }

  //AWS 설정
  wifiClient.setTrustAnchors(&ca);
  wifiClient.setClientRSACert(&cert, &key);
  setClock();
  client.setCallback(callback);
  //Serial.println("Certifications and key are set");
  Serial.println("AWS connected");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(thingId)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(outTopic, "hello world");
      sendMqtt();
      // ... and resubscribe
      client.subscribe(inTopic);
      //oled에 표시
      oled.setTextXY(6,4);     
      oled.putString("AWS on  ");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      wifiClient.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      //delay(5000);
    }
  }
}

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void AWSmqtt() {
  if (!client.connected()) {
    //oled에 표시
    oled.setTextXY(6,4);     
    oled.putString("AWS off");
    reconnect();
  }
  client.loop();
}

void loop() {
  //timer.tick(); // tick the timer
  server.handleClient();

  if(WiFi.status()==WL_CONNECTED) {
    AWSmqtt();
    //Serial.println("-----------------");
  }
  
  if( isr_flag == 1 ) {
    detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    //attachInterrupt(0, interruptRoutine, FALLING);
    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
  }
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("UP");
        relayOff();
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        relayOff();
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        relayOn();
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        relayOn();
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        break;
      case DIR_FAR:
        Serial.println("FAR");
        break;
      default:
        Serial.println("NONE");
    }
  }
}

void relayOn() {
  p4[0]=1;
  digitalWrite(relay,HIGH);
  Serial.println("on");
  sendMqtt();
}

void relayOff() {
  p4[0]=0;
  digitalWrite(relay,LOW);
  Serial.println("off");
  sendMqtt();
}

void sendMqtt() {
  if(WiFi.status()==WL_CONNECTED) {
    snprintf(msg, 100, "{\"chip\":\"%s\",\"on\":%d}",cChipId,p4[0]);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(outTopic, msg);
    delay(200);
  }
}
