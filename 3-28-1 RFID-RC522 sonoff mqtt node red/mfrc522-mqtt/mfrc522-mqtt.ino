/**
d1 mini rc52 wiring
RST  - D3
MISO - D6
MOSI - D7
SCK  - D5
SDA  - D8
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN =  0;          // Configurable, see typical pin layout above 18
constexpr uint8_t SS_PIN =  15;         // Configurable, see typical pin layout above  16

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// 아래의 6개설정은 사용자 환경에 맞게 수정하세요.
const char* ssid = "i2r"; // 와이파이 AP, 또는 스마트폰의 핫스판 이름
const char* password = "00000000";  // 와이파이 AP, 또는 스마트폰의 핫스판 이름
const char* mqtt_server = "192.168.0.200"; //브로커 주소
const char* outTopic = "/rfid/outTopic"; // 이름이 중복되지 않게 설정 기록
const char* inTopic = "/rfid/inTopic"; // 이름이 중복되지 않게 설정 기록
const char* clientName = "";  // setup 함수에서 자동생성
String sChipID;
char cChipID[20];

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup() {
  Serial.begin(38400);
  setup_wifi();

  //이름 자동으로 생성
  sChipID=String(ESP.getChipId(),HEX);
  sChipID.toCharArray(cChipID,sChipID.length()+1);
  clientName=&cChipID[0];
  Serial.println(clientName);
  
  //delay(1000);
  //Serial.println("Setup");
  
  //while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  Serial.println("Setup done");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  // We start by connecting to a WiFi network
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다.
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// mqtt 통신에 지속적으로 접속한다.
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
//  Serial.println("Loop...");

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    //delay(50);
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    //delay(50);
    return;
  }

  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  
  // Dump debug info about the card; PICC_HaltA() is automatically called
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  delay(1000);
}

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  String card;
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    card+=String(buffer[i],HEX);
  }
  card.toUpperCase();
  Serial.println("");
  Serial.println(card);
  Serial.println(bufferSize);
  char bufferCard[20];
  card.toCharArray(bufferCard,9);
  snprintf (msg, 75, "{\"rfid\":\"%s\"}",bufferCard);
  Serial.println(msg);
  client.publish(outTopic, msg);
} 
