// CO2 센서
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "i2r";
const char* password = "00000000";
String ipWeb="192.168.0.101";

String inputString = "";         // a string to hold incoming data
String webString = "xxaaxxaa";
String strID = "01";
boolean stringComplete = false;  // whether the string is complete

ESP8266WebServer server(80);

void handleRoot() {
   char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  // 계측된 센서값을 정리한다. 센서갑만 추려낸다.
  int str_len = webString.length() + 1; 
  char tx[str_len];
  webString.toCharArray(tx, str_len);
  for(int i=0;i<5;i++)
     tx[i]=tx[i+2];
  tx[6]=0;
  Serial.println(tx);
  
  snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>CO2 Sensor</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>CO2 Sensor  Web Service!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>Value: %s</p>\
  </body>\
</html>",

    hr, min % 60, sec % 60, tx
  );
  server.send ( 200, "text/html", temp );
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  Serial.begin(9600);
  //고정 IP 설정
  IPAddress ip(192, 168, 0, 101); 
  IPAddress gateway(117, 16, 177, 126);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet); // before or after Wifi.Begin(ssid, password);
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

  server.on("/", handleRoot);
  server.on("/value", [](){
    server.send(200, "text/plain", "CO2 Sensor Value : "+webString.substring(2,8)+" [ID "+strID+"]" + 
             "<A HREF=\"http://"+ipWeb+"/\"> "+ ipWeb +"  \</A>");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop(void){
  // print the string when a newline arrives:
  serialEvent();
  if (stringComplete) {
    Serial.println(inputString);
    webString=inputString;
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  server.handleClient();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
