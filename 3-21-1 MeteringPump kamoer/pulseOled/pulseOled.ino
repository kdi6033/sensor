// 회로도 및 보드 구매 : 통신보드 구매사이트 : https://smartstore.naver.com/i2r/products/4584482642
// plc에서 html 작성 참조 : https://youtu.be/Ule_wKCxJZc
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <arduino-timer.h>
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>

//auto timer = timer_create_default(); // create a timer with default settings
Timer<1, micros> timer; 
String sChipId="";
char cChipId[40]="";
ESP8266WebServer server(80);

//String inputString = "";         // 받은 문자열
//String s;
//int LED = LED_BUILTIN;// 사용하지 마세요 serial1 통신과 충돌합니다.

//Step Motor OK42STH45-1684AYT  7TPSM4220
// 1 pulse  1.8도/indexer 만큼 회전
//indexer 1=200 pulse, 2=400 pulse, 4=800 pulse, 8=1600 pulse, 
// 16=3200 pulse,32=6400 pulse 를 보내면 motor는 1회전 합니다.
int DIR = D0; // 회전방향
int STP = D3; // pulse
int EN = D4;  // step 진행 on off

unsigned long count=0,countPulse=0,lastCount=0;
int delayMsec=0; // 100이상해야함
int onSw=0;
bool toggle_led(void *) {
  rotate();
  return true; 
}


void rotate() {
  if(onSw==0)
    return;
  count++;
  if(count==1) {
    digitalWrite(STP, 1);
    countPulse++;
    //Serial.println(countPulse);
  }
  else if(count==51)
    digitalWrite(STP, 0);
  if(countPulse > 256) {
    digitalWrite(EN, 0);
    delay(1);
    digitalWrite(EN, 1);
    countPulse=0;
  }
  if(count>delayMsec)
    count=0;
}


/*
void rotate(int dir, int pulse) {
  //Serial.println("start");
  //digitalWrite(DIR, dir);
  //digitalWrite(EN, 1);
  int count=0;
  for(int i=0;i<pulse;i++) {
      digitalWrite(STP, 1);
      delayMicroseconds(700);
      //delay(delayMsec);
      digitalWrite(STP, 0);
      delayMicroseconds(300);
      delay(delayMsec);
      count++;
      if(count > 256) {
        digitalWrite(EN, 0);
        delay(1);
        digitalWrite(EN, 1);
        count=0;
      }
  }
}
*/

void setup() {
  // call the toggle_led function every 1000 millis (1 second)
  timer.every(1, toggle_led);
  pinMode(DIR, OUTPUT); 
  pinMode(STP, OUTPUT);
  pinMode(EN, OUTPUT);
  
  digitalWrite(DIR, 0);
  digitalWrite(EN, 0);
  digitalWrite(STP, 0);
  delayMsec=200;

  Wire.begin();  
  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //pinMode(LED, OUTPUT);

  // AP 이름 자동으로 만듬 i2r-chipid
  sChipId = "i2r-"+String(ESP.getChipId(),HEX);
  sChipId.toCharArray(cChipId,sChipId.length()+1);
  Serial.println(sChipId);

  //WiFiManager
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  //wm.resetSettings(); //reset settings - for testing

  if (!wm.autoConnect(cChipId)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected... :)");
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

}

void loop() {
  timer.tick(); // tick the timer
  server.handleClient();
  //rotate(1, 200);
  /*
  long now = millis();
  int out;
  if(now < (lastCount + 7000))
    out=1;
  else
    out=0;
  Serial.println(out);
  digitalWrite(STP, out);
  if (now > (lastCount + 10000)) 
    lastCount=now;
    */
}
