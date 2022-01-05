import socket

a_socket = socket.socket()

try:
    a_socket.connect(("127.0.0.1", 9600))
    payload = "Sec-WebSocket-Key: ABASDSF"
    a_socket.send(payload.encode())
    print(a_socket.recv(2048))
except:
    print("Connection failed")
