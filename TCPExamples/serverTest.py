#!/usr/bin/env python

import socket


TCP_IP = '127.0.0.1'
TCP_PORT = 8080
BUFFER_SIZE = 20  # Normally 1024, but we want fast response
MESSAGE = '12345678901234567890'

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn, addr = s.accept()
print('Connection address:', addr)
while 1:
    # data = conn.recv(BUFFER_SIZE)
    # if not data: break
    # print("received data:", data)
    conn.send(MESSAGE.encode())  # echo
conn.close()
