import zmq
import os
import sys
import time

try:
    server_pub = os.environ["SERVER_PUBLIC_KEY"].encode('ascii')
    client_pub = os.environ["CLIENT_PUBLIC_KEY"].encode('ascii')
    client_sec = os.environ["CLIENT_SECRET_KEY"].encode('ascii')
except:
    pass

context = zmq.Context()
#socket = context.socket(zmq.SUB)
socket = context.socket(zmq.PULL)

#socket.setsockopt(zmq.CURVE_SERVER, True)
try:
    socket.setsockopt(zmq.CURVE_PUBLICKEY, client_pub)
    socket.setsockopt(zmq.CURVE_SECRETKEY, client_sec)
    socket.setsockopt(zmq.CURVE_SERVERKEY, server_pub)
except: 
    pass
#socket.setsockopt(zmq.SUBSCRIBE, b"")
adrr = sys.argv[1]
socket.connect(adrr)
print(f"listening the {adrr}")
cnt = 0

start_time = time.perf_counter()

for i in range(0, 100000):
#while True:
    msg = socket.recv_string()
    print(f"Received the {cnt}th message with content {msg}")
    cnt += 1
end_time = time.perf_counter()

elapsed_time = end_time - start_time
print(f"SENT {cnt} messages in {elapsed_time:.6f} seconds")

