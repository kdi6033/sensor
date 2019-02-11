#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Replace with your network credentials
const char* ssid = "i2r";
const char* password = "00000000";

ESP8266WebServer server(80);

String webPage = "";

int gpio13Led = 13;
int gpio12Relay = 12;

void connectWifi() {
  Serial.println("Connecting as wifi client...");
  WiFi.disconnect();
  WiFi.begin ( ssid, password );
  
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

void handleRoot() {
  server.send(200, "text/html", webPage);
}

void handleOn() {
  server.send(200, "text/html", webPage);
  digitalWrite(gpio13Led, LOW);
  digitalWrite(gpio12Relay, HIGH);
  delay(1000);
}

void handleOff() {
  server.send(200, "text/html", webPage);
  digitalWrite(gpio13Led, HIGH);
  digitalWrite(gpio12Relay,LOW);
  delay(1000);
}

void setup(void){
  webPage += "<meta name='viewport' content='width=device-width, initial-scale=1.0'/>";
  webPage += "<meta http-equiv='Content-Type' content='text/html;charset=utf-8' />";
  webPage += "<h1>SONOFF 웹서버</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a></p>";  
  // preparing GPIOs
  pinMode(gpio13Led, OUTPUT);
  digitalWrite(gpio13Led, HIGH);
  pinMode(gpio12Relay, OUTPUT);
  digitalWrite(gpio12Relay, HIGH);
  Serial.begin(115200); 
  delay(2000);
  
  connectWifi();
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
 
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
} 

