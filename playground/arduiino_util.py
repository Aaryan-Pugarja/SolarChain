from flask import Flask
from flask_sock import Sock

app = Flask(__name__)
sock = Sock(app)

esp_client = None


@sock.route("/")
def websocket(ws):
    global esp_client

    print("Client connected")

    while True:
        message = ws.receive()

        if message is None:
            break

        print("Received:", message)

        if message == "ESP_CONNECTED":
            esp_client = ws
            print("ESP registered")
            continue

        # Forward browser command to ESP
        if esp_client and ws != esp_client:
            esp_client.send(message)

    print("Client disconnected")

    if ws == esp_client:
        esp_client = None


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
