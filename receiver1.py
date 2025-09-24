import zmq

context = zmq.Context()
socket = context.socket(zmq.PULL)
socket.bind("tcp://localhost:6001")

while True:
    msg = socket.recv_string()
    print(f"Received the message: {msg}")

