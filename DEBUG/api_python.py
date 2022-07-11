import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("127.0.0.1", 25000))
sock.sendall(bytes("15", "utf-8"))
