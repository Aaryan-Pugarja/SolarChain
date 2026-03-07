import websocket

ws = websocket.WebSocket()
ip = "10.216.152.154"
ws.connect(f"ws://{ip}:8080/")

ws.send("on")
# ws.send("off")

ws.close()