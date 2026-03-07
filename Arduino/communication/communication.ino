#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "Monchu 2.4G";
const char* password = "arpit2005@";

const char* host = "192.168.0.19";
const uint16_t port = 8080;

WebSocketsClient webSocket;

const int relayPin = 5;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {

    case WStype_CONNECTED:
      Serial.println("Connected to Flask WebSocket server");
      webSocket.sendTXT("ESP_CONNECTED");
      break;

    case WStype_DISCONNECTED:
      Serial.println("Disconnected from server");
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

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  webSocket.begin(host, port, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}