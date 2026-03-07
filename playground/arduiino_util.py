from flask import Flask
from flask_sock import Sock

app = Flask(__name__)
sock = Sock(app)

nodes = {}

SENDER_MAC = "1C:C3:AB:A2:17:40"
RECEIVER_MAC = "00:70:07:3A:63:D4"


@sock.route("/")
def websocket(ws):

    mac = None

    while True:

        message = ws.receive()

        if message is None:
            break

        print("Received:", message)

        if message.startswith("REGISTER:"):

            mac = message.split(":")[1:]
            mac = ":".join(mac)

            nodes[mac] = ws

            if mac == SENDER_MAC:
                ws.send("ROLE:sender")

            elif mac == RECEIVER_MAC:
                ws.send("ROLE:receiver")

            continue


        if message.startswith("UNITS:"):

            if mac == SENDER_MAC:

                if RECEIVER_MAC in nodes:

                    nodes[RECEIVER_MAC].send(message)

                    print("Forwarded units to receiver")

    if mac in nodes:
        del nodes[mac]


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
