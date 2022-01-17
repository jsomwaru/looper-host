import socket

a_socket = socket.socket()

try:
    a_socket.connect(("127.0.0.1", 9600))
    #upgrade
    payload = "Sec-WebSocket-Key: ABASDSF"
    #a_socket.send(payload.encode())
    me = "jacob" * 554
    a_socket.send(me.encode())
except:
    print("Connection failed")
