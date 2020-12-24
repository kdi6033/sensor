// 회로도 및 보드 구매 : 통신보드 구매사이트 : https://smartstore.naver.com/i2r/products/4584482642
// plc에서 html 작성 참조 : https://youtu.be/Ule_wKCxJZc
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <arduino-timer.h>
#include <ACROBOTIC_SSD1306.h>
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

bool toggle_led(void *) {

  return true; 
}

void setup() {
  // call the toggle_led function every 1000 millis (1 second)
  timer.every(5000, toggle_led);
  
  // put your setup code here, to run once:
  Serial.begin(115200);

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
}

void loop() {
  //timer.tick(); // tick the timer
  server.handleClient();
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
  digitalWrite(relay,LOW);
  Serial.println("on");
}

void relayOff() {
  digitalWrite(relay,HIGH);
  Serial.println("off");
}
