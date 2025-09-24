import zmq

context = zmq.Context()
socket = context.socket(zmq.PULL)
socket.bind("tcp://localhost:6002")

while True:
    msg = socket.recv()
    print(f"Received the message: {msg}")

