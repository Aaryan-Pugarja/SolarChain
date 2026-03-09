#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "Monchu 2.4G";
const char* password = "arpit2005@";

const char* host = "192.168.0.19";
const uint16_t port = 5000;

WebSocketsClient webSocket;

String role = "unknown";
String mac;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
    switch(type)
    {

        case WStype_CONNECTED:
        {
            Serial.println("Connected to server");

            mac = WiFi.macAddress();
            webSocket.sendTXT("REGISTER:" + mac);

            break;
        }

        case WStype_TEXT:
        {
            String msg = String((char*)payload);

            Serial.print("Received: ");
            Serial.println(msg);

            if (msg.startsWith("ROLE:"))
            {
                role = msg.substring(5);

                Serial.print("Role assigned: ");
                Serial.println(role);
            }

            if (msg.startsWith("UNITS:"))
            {
                int units = msg.substring(6).toInt();

                Serial.print("Units Input: ");
                Serial.println(units);

                if (role == "sender")
                {
                    Serial.println("Sender received units command");
                }

                if (role == "receiver")
                {
                    Serial.println("Receiver acknowledged units");
                }

                webSocket.sendTXT("UNITS_RECEIVED");
            }

            break;
        }

        case WStype_DISCONNECTED:
        {
            Serial.println("Disconnected from server");
            break;
        }

        default:
            break;
    }
}

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");

    webSocket.begin(host, port, "/");
    webSocket.onEvent(webSocketEvent);
}

void loop()
{
    webSocket.loop();
}
