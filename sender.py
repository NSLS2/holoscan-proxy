import zmq
import time

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.bind("tcp://localhost:5555")

while True:
    msg = f"Test message at {time.time()}\n"
    print("sending: ", msg)
    socket.send_string(msg)
    time.sleep(0.1)
