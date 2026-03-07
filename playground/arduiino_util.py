from flask import Flask
from flask_sock import Sock

app = Flask(__name__)
sock = Sock(app)

esp_clients = {}

sender_mac = "1C:C3:AB:A2:17:40"
receiver_mac = "00:70:07:3A:63:D4"


@sock.route("/")
def websocket(ws):

    device_mac = None
    print("Client connected")

    while True:

        message = ws.receive()

        if message is None:
            break

        print("Received:", message)

        if message.startswith("ESP_CONNECTED:"):
            device_mac = message.split(":")[1:]
            device_mac = ":".join(device_mac)

            esp_clients[device_mac] = ws

            print("Registered ESP:", device_mac)
            continue

        if ws not in esp_clients.values():

            if message == "sender_on":
                if sender_mac in esp_clients:
                    esp_clients[sender_mac].send("on")

            if message == "sender_off":
                if sender_mac in esp_clients:
                    esp_clients[sender_mac].send("off")

            if message == "receiver_on":
                if receiver_mac in esp_clients:
                    esp_clients[receiver_mac].send("on")

            if message == "receiver_off":
                if receiver_mac in esp_clients:
                    esp_clients[receiver_mac].send("off")

    print("Client disconnected")

    if device_mac and device_mac in esp_clients:
        del esp_clients[device_mac]


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
