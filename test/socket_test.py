import socket

a_socket = socket.socket()

try:
    a_socket.connect(("127.0.0.1", 9600))
    payload = "hi" * 2048
    a_socket.send(payload.encode())
except:
    print("Connection failed")
