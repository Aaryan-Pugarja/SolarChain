#include <Wire.h>
#include <7Semi_INA219.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define CONTROL_PIN 5

#define JOULES_PER_UNIT 5.0

INA219_7Semi ina(0x40);

const char* ssid = "vivo 1951";
const char* password = "arpit2005";

const char* host = "192.168.0.19";
const uint16_t port = 5000;

WebSocketsClient webSocket;

String role = "unknown";
String mac;

float totalEnergy_J = 0.0;
float targetEnergy_J = 0.0;

unsigned long lastTime = 0;

bool transferActive = false;

void startReceive(float joules)
{
    targetEnergy_J = joules;
    totalEnergy_J = 0;

    lastTime = millis();

    digitalWrite(CONTROL_PIN, LOW);

    transferActive = true;

    Serial.print("Receiving energy target: ");
    Serial.print(joules);
    Serial.println(" J");
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

                Serial.print("Units Input: ");
                Serial.println(units);

                float joules = units * JOULES_PER_UNIT;

                Serial.print("Converted Joules: ");
                Serial.println(joules);

                startReceive(joules);

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

    pinMode(CONTROL_PIN, OUTPUT);
    digitalWrite(CONTROL_PIN, HIGH);

    if (!ina.begin(&Wire))
    {
        Serial.println("INA219 not found");
        while(1);
    }

    ina.configure(false, 3, 0x0B, 0x0B, 0x07);
    ina.calibrateAuto(2.0, 0.1);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");

    webSocket.begin(host, port, "/");
    webSocket.onEvent(webSocketEvent);

    lastTime = millis();
}

void loop()
{
    webSocket.loop();

    if (transferActive && ina.conversionReady())
    {
        float power_mW = ina.readPower();

        unsigned long now = millis();
        float dt_seconds = (now - lastTime) / 1000.0;
        lastTime = now;

        float power_W = power_mW / 1000.0;

        totalEnergy_J += power_W * dt_seconds;

        Serial.print("Power: ");
        Serial.print(power_W, 3);
        Serial.print(" W   Energy: ");
        Serial.print(totalEnergy_J, 3);
        Serial.print(" / ");
        Serial.print(targetEnergy_J, 3);
        Serial.println(" J");

        if (totalEnergy_J >= targetEnergy_J)
        {
            digitalWrite(CONTROL_PIN, HIGH);

            transferActive = false;

            Serial.println("Energy received target reached");

            webSocket.sendTXT("RECEIVE_COMPLETE");
        }
    }

    delay(200);
}
