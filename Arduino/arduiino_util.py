from flask import Flask, request, jsonify
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


@app.route("/transfer", methods=["POST"])
def transfer_units():

    data = request.json
    units = int(data["units"])

    if SENDER_MAC not in nodes:
        return jsonify({"error": "Sender not connected"}), 400

    if RECEIVER_MAC not in nodes:
        return jsonify({"error": "Receiver not connected"}), 400

    msg = f"UNITS:{units}"

    nodes[SENDER_MAC].send(msg)
    nodes[RECEIVER_MAC].send(msg)

    print("Transfer started:", units)

    return jsonify({
        "status": "transfer_started",
        "units": units
    })


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)
