#include <ESP8266WiFi.h>

const char* ssid = "i2r";
const char* password = "00000000";

String inputString = "";         // a string to hold incoming data
String webString = "";
String commandString = ":80040004000276";
boolean stringComplete = false;  // whether the string is complete

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(10);
  // 온습도 요청 명령문 CR(13), LF(10) 추가 이보드는 자동으로 붙는다.
  //commandString += char(13);
  //commandString += char(10);
  //commandString += char(0);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.println(WiFi.localIP());
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  requestTX();
  delay(4000);
  // print the string when a newline arrives:
  serialEvent();
  if (stringComplete) {
    Serial.println(inputString);
    webString=inputString;
    // clear the string
    inputString = "";
    stringComplete = false;
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  
  client.flush();
  // Prepare the response
  client.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n");
  //정기적으로 페이지 업데이트
  client.println("<head>\r\n<meta http-equiv='refresh' content='5'/>\r\n");
  client.println("<title>Humidity & Teperature</title>\r\n<style>\r\n");
  client.println("body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\r\n");
  client.println("</style>\r\n</head>\r\n");
  client.println("<body>");
  client.println("<br />");
  client.println("<h1>");
  // 온도와 습도 표시
  client.println(webString);
  client.println("</h1>");
  client.println("</body>");
  client.println("</html>");  
  
  delay(1);
  Serial.println("Client disonnected");
  
}

void requestTX() {
  Serial1.println(commandString);
}

// 시리얼통신으로 문자가 들어오면 처리한다.
void serialEvent() {
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    String inputString="";
    for(int i=0;i<len;i++)
      inputString += char(sbuf[i]);
    Serial.println(inputString);
    if( inputString.startsWith(":800404") == true) {
      int Humi, Temp;
      Serial.println("OK");
      Humi = HexToInt(sbuf[7])*16*16*16 + HexToInt(sbuf[8])*16*16 + 
      HexToInt(sbuf[9])*16 + HexToInt(sbuf[10]);
      Serial.println((float)Humi/100);
      Temp = HexToInt(sbuf[11])*16*16*16 + HexToInt(sbuf[12])*16*16 + 
      HexToInt(sbuf[13])*16 + HexToInt(sbuf[14]);
      Serial.println((float)Temp/100);
      webString = " Humidity: " + String((float)Humi/100) + "<br /> Temperature: " + String((float)Temp/100);
    }
  }
}

int HexToInt(char hex) {
  if( hex <= '9')
    return hex - '0';
  else if (hex <= 'F')
    return hex-'A' +10;
  else if (hex <= 'f')
    return hex-'a' +10;
}



