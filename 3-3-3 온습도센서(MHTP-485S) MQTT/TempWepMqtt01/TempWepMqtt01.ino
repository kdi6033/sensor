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

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial1.begin(9600);
  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}


void setup_wifi() {
  delay(10);
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
  // Print the IP address
  Serial.println(WiFi.localIP());  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientName)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Reconnected");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  CheckSerial();
  //CheckWeb();
  CheckMqtt();
}

void CheckMqtt() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
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
      // 온도와 습도 값을 브로커에 보낸다.
      sprintf (msg, "\{Humidity: %f, Temperature: %f\}", (float)Humi/100, (float)Temp/100);
      client.publish(outTopic, msg);
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



