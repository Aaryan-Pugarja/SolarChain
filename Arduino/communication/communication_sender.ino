#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Wire.h>
#include <7semi_MAX17048.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define LOAD_PIN 5

#define BATTERY_ENERGY_J 26640.0
#define JOULES_PER_UNIT 5.0

MAX17048_7semi battery;

bool sensorReady = false;
bool draining = false;

float startSOC = 0;
float targetDrop = 0;

const char* ssid = "vivo 1951";
const char* password = "arpit2005";

const char* host = "192.168.0.19";
const uint16_t port = 5000;

WebSocketsClient webSocket;

String role = "unknown";
String mac;

void startDrain(float joules)
{
    float soc = battery.cellPercent();

    targetDrop = (joules / BATTERY_ENERGY_J) * 100.0;

    startSOC = soc;

    Serial.print("Starting drain for Joules: ");
    Serial.println(joules);

    Serial.print("Target SOC drop: ");
    Serial.println(targetDrop);

    digitalWrite(LOAD_PIN, HIGH);

    draining = true;
}

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

                Serial.print("Units received: ");
                Serial.println(units);

                float joules = units * JOULES_PER_UNIT;

                Serial.print("Converted Joules: ");
                Serial.println(joules);

                startDrain(joules);

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

    Wire.begin(SDA_PIN, SCL_PIN);

    pinMode(LOAD_PIN, OUTPUT);
    digitalWrite(LOAD_PIN, LOW);

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
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.print(" V | SOC: ");
    Serial.print(soc);
    webSocket.loop();

    if (!sensorReady)
    {
        if (battery.begin(&Wire))
        {
            Serial.println("MAX17048 initialized");
            battery.quickStart();
            battery.setVoltageLimits(3.2, 4.2);
            sensorReady = true;
        }
        else
        {
            Serial.println("MAX17048 not detected...");
            delay(1000);
            return;
        }
    }

    float soc = battery.cellPercent();
    float voltage = battery.cellVoltage();

    if (draining)
    {
        float drop = startSOC - soc;

        Serial.print("Voltage: ");
        Serial.print(voltage);
        Serial.print(" V | SOC: ");
        Serial.print(soc);
        Serial.print(" % | Drop: ");
        Serial.println(drop);

        if (drop >= targetDrop)
        {
            digitalWrite(LOAD_PIN, LOW);
            draining = false;

            Serial.println("Target energy drained. Load OFF.");

            webSocket.sendTXT("TRANSFER_COMPLETE");
        }
    }

    delay(1000);
}