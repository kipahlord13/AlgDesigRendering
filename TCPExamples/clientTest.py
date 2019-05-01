#!/usr/bin/env python

import socket


TCP_IP = '127.0.0.1'
TCP_PORT = 8080
BUFFER_SIZE = 1024
MESSAGE = "Hello, World!"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))



s.send(MESSAGE.encode())
print("Message sent")

data = s.recv(BUFFER_SIZE)
print("received data:", data)

s.close()
