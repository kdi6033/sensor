#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Servo.h> 


const char* ssid = "i2r";
const char* password = "00000000";
ESP8266WebServer server(80);

StaticJsonDocument<200> doc;
DeserializationError error;
JsonObject root;
String payload;

int pos=0;
char msg[100];

Servo myservo; 

String s,s_head;
void handleRoot() {
  s="";
  s=s+"{\"pos\":"+pos+"}";
  server.send(200, "text/plain", s);
  //server.send(200, "text/html", s);
  
}

// 크라이언트는 주소창에 http://IP주소/json?j={"pos":각도} 라고 입력
void handleJson() {
  server.arg("j").toCharArray(msg, 99);
  Serial.println(msg);
  deserializeJson(doc,msg);
  root = doc.as<JsonObject>();
  pos = root["pos"];
  myservo.write(pos); 
  Serial.println(pos);

  handleRoot();
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  myservo.attach(2);  // attaches the servo on GIO2 to the servo object 
  Serial.begin(9600);
  setupWifi();
  server.on("/", handleRoot);
  server.on("/json", handleJson);
  server.begin();
  Serial.println("HTTP server started");
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

}
