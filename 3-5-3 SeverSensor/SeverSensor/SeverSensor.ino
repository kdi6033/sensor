// 센서 서버
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
 
const char* ssid = "405902-2.4G";
const char* password = "k01033887147";
String strWeb="";

ESP8266WebServer server(80);

void handleRoot() {
    char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  // 계측된 센서값을 정리한다. 
  int str_len = strWeb.length() + 1; 
  char tx[str_len];
  strWeb.toCharArray(tx, str_len);
  //Serial.println(tx);
  
  snprintf ( temp, 400,
"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Sensor Sever</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1> Sensor  Web Service!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p> %s</p>\
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

void setup () {
  Serial.begin(9600);
  //고정 IP 설정
  IPAddress ip(172, 30, 1, 100); 
  IPAddress gateway(172, 30, 1, 254);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet); // before or after Wifi.Begin(ssid, password);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
   while (WiFi.status() != WL_CONNECTED) {
     delay(1000);
    Serial.print("Connecting..");
   }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // DNS를 i2r.local로 설정
  if (MDNS.begin("i2r")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop() {
  server.handleClient();
  strWeb="";
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      for(int i=101;i<103;i++) {
         HTTPClient http;  //Declare an object of class HTTPClient
         //http.begin("http://jsonplaceholder.typicode.com/users/1");  //Specify request destination
        //http.begin("http://192.168.0.101/value"); 
        String strSensor="http://172.30.1.";
        strSensor += i;
        strSensor += "/value";
        Serial.println(strSensor);  
        http.begin(strSensor);
        int httpCode = http.GET();                                                                  //Send the request
         if (httpCode > 0) { //Check the returning code
           strWeb += "<p>";
           strWeb += http.getString();   //Get the request response payload
           strWeb += "</p>";
         }
         http.end();   //Close connection
      }
      Serial.println(strWeb);                     //Print the response payload
     }
   delay(3000);    //Send a request every 30 seconds
}
