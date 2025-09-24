import zmq
import os

server_pub = os.environ["SERVER_PUBLIC_KEY"].encode()
client_pub = os.environ["CLIENT_PUBLIC_KEY"].encode()
client_sec = os.environ["CLIENT_SECRET_KEY"].encode()

context = zmq.Context()
socket = context.socket(zmq.PULL)
socket.setsockopt(zmq.CURVE_PUBLICKEY, client_pub)
socket.setsockopt(zmq.CURVE_SECRETKEY, client_sec)
socket.setsockopt(zmq.CURVE_SERVERKEY, server_pub)

socket.bind("tcp://localhost:6002")

while True:
    msg = socket.recv_string()
    print(f"Received the message: {msg}")

