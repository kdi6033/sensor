/* D3(GPIO0,Pull-up) 핀이 Max485의 RE DE 핀에 연결되어 있다. - 쓰기모드
 * D3 = LOW 일때 읽기모드이다.
 */

#include <ESP8266WiFi.h>

const char* ssid = "i2r";
const char* password = "00000000";

String inputString = "";         // a string to hold incoming data
String webString = "";
String commandString = ":80040004000276";
boolean stringComplete = false;  // whether the string is complete

const int contPin =  0;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  pinMode(contPin, OUTPUT);
  digitalWrite(contPin, LOW); // RS485 읽기 모드 
  //digitalWrite(contPin, HIGH); // RS485 쓰기 모드 
  
  // initialize serial:
  Serial.begin(9600);
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
          client.println("<body>");
          client.println("<br />");
          client.println(webString);
          client.println("</body>");
          client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  
}

// 시리얼통신으로 문자가 들어오면 처리한다.
void serialEvent() {
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    inputString="";
    for(int i=0;i<len;i++)
      inputString += char(sbuf[i]);
    Serial.println(inputString);
  }
}

/*
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // LF (아스키번호 10)이 들어오면 하나의 문장처리
    if (inChar == char(10)) {
      stringComplete = true;
    }
    Serial.println(Serial.available());
    Serial.println(inChar);
  }
}
*/

void requestTX() {
  digitalWrite(contPin, HIGH);
  delay(1000);
  Serial.println(commandString);
  delay(100);
  digitalWrite(contPin, LOW);
  delay(100);
}


