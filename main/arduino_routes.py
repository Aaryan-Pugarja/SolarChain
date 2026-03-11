from flask_sock import Sock

from main import app

sock = Sock(app)

SENDER_MAC = "F8:B3:B7:7A:68:64"
RECEIVER_MAC = "AC:15:18:D6:0F:7C"
nodes = {}

@sock.route("/")
def websocket(ws):

    mac = None

    while True:

        message = ws.receive()

        if message is None:
            break

        print("Received:", message)

        if message.startswith("REGISTER:"):

            mac = ":".join(message.split(":")[1:])
            nodes[mac] = ws

            if mac == SENDER_MAC:
                ws.send("ROLE:sender")

            elif mac == RECEIVER_MAC:
                ws.send("ROLE:receiver")

            print("Registered:", mac)
            continue

        if message in ["UNITS_RECEIVED", "DRAIN_COMPLETE"]:
            print("Transfer acknowledgement received")

    if mac and mac in nodes:
        del nodes[mac]
