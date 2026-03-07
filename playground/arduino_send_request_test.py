import websocket

ws = websocket.WebSocket()
ws.connect("ws://192.168.0.19:8080/")

ws.send("on")
# ws.send("off")

ws.close()