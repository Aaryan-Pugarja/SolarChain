from main.arduino_routes import RECEIVER_MAC, SENDER_MAC, nodes


def initiate_transfer(units):

    msg = f"UNITS:{units}"

    if SENDER_MAC in nodes:
        nodes[SENDER_MAC].send(msg)

    if RECEIVER_MAC in nodes:
        nodes[RECEIVER_MAC].send(msg)
