#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "vivo 1951";
const char* password = "arpit2005";

const char* host = "10.216.152.154";
const uint16_t port = 8080;

WebSocketsClient webSocket;

const int relayPin = 5;

String deviceId;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {

    case WStype_CONNECTED:
      Serial.println("Connected to server");

      deviceId = WiFi.macAddress();
      webSocket.sendTXT("ESP_CONNECTED:" + deviceId);

      break;

    case WStype_DISCONNECTED:
      Serial.println("Disconnected");
      break;

    case WStype_TEXT: {

      String msg = String((char*)payload);

      Serial.print("Received: ");
      Serial.println(msg);

      if (msg == "on") {
        digitalWrite(relayPin, HIGH);
      }

      if (msg == "off") {
        digitalWrite(relayPin, LOW);
      }

      break;
    }

    default:
      break;
  }
}

void setup() {

  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  webSocket.begin(host, port, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}