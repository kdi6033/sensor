#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "i2r";
const char* password = "00000000";

String inputString = "";         // a string to hold incoming data
String webString = "";
String commandString = ":80040004000276";
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* outTopic = "outTopic/kdi/";
const char* inTopic = "inTopic/kdi/";
const char* clientName = "D1miniClientA/kdi/";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0,lastMsgMqtt = 0;
char msg[50];
int value = 0;

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
  //requestTX();
  //delay(4000);
  //serialEvent();
  
  CheckSerial();
  CheckWeb();
}

void CheckSerial() {
  long now = millis();
  if (now - lastMsg > 4000) {
    lastMsg = now;
    Serial1.println(commandString);
  }
  // print the string when a newline arrives:
  serialEvent();
}

void CheckWeb() {
  // Check if a espClient has connected
  espClient = server.available();
  if (!espClient) {
    return;
  }

  // Wait until the espClient sends some data
  Serial.println("new Client");
  // Read the first line of the request
  String req = espClient.readStringUntil('\r');
  Serial.println(req);
  
  espClient.flush();
  // Prepare the response
  espClient.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n");
  //정기적으로 페이지 업데이트
  espClient.println("<head>\r\n<meta http-equiv='refresh' content='5'/>\r\n");
  espClient.println("<title>Humidity & Teperature</title>\r\n<style>\r\n");
  espClient.println("body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\r\n");
  espClient.println("</style>\r\n</head>\r\n");
  espClient.println("<body>");
  espClient.println("<br />");
  espClient.println("<h1>");
  // 온도와 습도 표시
  espClient.println(webString);
  espClient.println("</h1>");
  espClient.println("</body>");
  espClient.println("</html>");  
  
  delay(1);
  Serial.println("Client disonnected");
    
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



