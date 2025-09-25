import zmq
import os

#server_pub = os.environ.get("SERVER_PUBLIC_KEY")
#client_pub = os.environ.get("CLIENT_PUBLIC_KEY")
#client_sec = os.environ.get("CLIENT_SECRET_KEY")

server_pub = os.environ["SERVER_PUBLIC_KEY"].encode('ascii')
client_pub = os.environ["CLIENT1_PUBLIC_KEY"].encode('ascii')
client_sec = os.environ["CLIENT1_SECRET_KEY"].encode('ascii')


context = zmq.Context()
socket = context.socket(zmq.SUB)
#socket = context.socket(zmq.PULL)

#socket.setsockopt(zmq.CURVE_SERVER, True)
socket.setsockopt(zmq.CURVE_PUBLICKEY, client_pub)
socket.setsockopt(zmq.CURVE_SECRETKEY, client_sec)
socket.setsockopt(zmq.CURVE_SERVERKEY, server_pub)

socket.setsockopt(zmq.SUBSCRIBE, b"")
socket.connect("tcp://localhost:6001")

while True:
    msg = socket.recv_string()
    print(f"Received the message: {msg}")

