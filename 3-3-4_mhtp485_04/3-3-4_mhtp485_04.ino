#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include <EEPROM.h>

MDNSResponder mdns;

/* Set these to your desired softAP credentials. They are not configurable at runtime */
const char *softAP_ssid = "i2r_ap_";
const char *softAP_password = "00000000";
String sAP_ssid;
char cAP_ssid[20];

/* hostname for mDNS. Should work at least on windows. Try http://i2r.local */
const char *myHostname = "i2r";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);


/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;

/** Current WLAN status */
unsigned int status = WL_IDLE_STATUS;

// rs232 통신을 위한 설정
String inputString = "";         // 수신 데이타 저장
boolean stringComplete = false;  // 수신 완료되면 true
String s;  // 보내는 문자열
int count,j;
long lastMsg = 0;
int delayTime=4000; //mqtt로 메세지 보낸는 시간 (단위 ms)
String commandString = ":80040004000276";
char msg[50];
float temperature,humidity;

void setup() {
  delay(1000);
  inputString.reserve(200);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  String ss;
  sAP_ssid=String(softAP_ssid)+String(ESP.getChipId(),HEX);
  sAP_ssid.toCharArray(cAP_ssid,sAP_ssid.length());
  softAP_ssid=&cAP_ssid[0];
  Serial.println(softAP_ssid);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */  
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server.on("/", handleRoot);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.onNotFound ( handleNotFound );
  server.begin(); // Web server start
  Serial.println("HTTP server started");
  loadCredentials(); // Load WLAN credentials from network
  connect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID
}

void connectWifi() {
  Serial.println("Connecting as wifi client...");
  WiFi.disconnect();
  WiFi.begin ( ssid, password );
  int connRes = WiFi.waitForConnectResult();
  Serial.print ( "connRes: " );
  Serial.println ( connRes );
}

void loop() {
  if (connect) {
    Serial.println ( "Connect requested" );
    connect = false;
    connectWifi();
    lastConnectTry = millis();
  }
  {
    unsigned int s = WiFi.status();
    if (s == 0 && millis() > (lastConnectTry + 60000) ) {
      /* If WLAN disconnected and idle try to connect */
      /* Don't set retry time too low as retry interfere the softAP operation */
      connect = true;
    }
    if (status != s) { // WLAN status change
      Serial.print ( "Status: " );
      Serial.println ( s );
      status = s;
      if (s == WL_CONNECTED) {
        /* Just connected to WLAN */
        Serial.println ( "" );
        Serial.print ( "Connected to " );
        Serial.println ( ssid );
        Serial.print ( "IP address: " );
        Serial.println ( WiFi.localIP() );

        if (mdns.begin("mhtp", WiFi.localIP())) {
            Serial.println("MDNS responder started");
        }

        // Setup MDNS responder
        if (!MDNS.begin(myHostname)) {
          Serial.println("Error setting up MDNS responder!");
        } else {
          Serial.println("mDNS responder started");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }
      } else if (s == WL_NO_SSID_AVAIL) {
        WiFi.disconnect();
      }
    }
  }
  // Do work:
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();

  CheckSerial();
}

void CheckSerial() {
  long now = millis();
  if (now - lastMsg > delayTime) {
    lastMsg = now;
    count++;
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
      humidity = (float)Humi/100;
      Serial.println(humidity);
      Temp = HexToInt(sbuf[11])*16*16*16 + HexToInt(sbuf[12])*16*16 + 
      HexToInt(sbuf[13])*16 + HexToInt(sbuf[14]);
      temperature = (float)Temp/100;
      Serial.println(temperature);
      // 온도와 습도 값을 브로커에 보낸다.
      sprintf (msg, "\{Humidity: %f, Temperature: %f\}", humidity, temperature);
      //client.publish(outTopic, msg);
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


/*
void serialEvent() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
  if(Serial.available() == false)
    return;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
  }
  stringComplete = true;
}
*/
