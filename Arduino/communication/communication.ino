#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "vivo 1951";
const char* password = "arpit2005";

const char* host = "10.216.152.154";
const uint16_t port = 8080;

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

                Serial.print("Energy units received: ");
                Serial.println(units);

                if (role == "receiver")
                {
                    Serial.println("Receiver accepting units");
                }
            }

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

    if (role == "sender")
    {
        static bool sent = false;

        if (!sent)
        {
            int units = 10;

            String msg = "UNITS:" + String(units);
            webSocket.sendTXT(msg);

            Serial.println("Sent units: " + String(units));

            sent = true;
        }
    }
}