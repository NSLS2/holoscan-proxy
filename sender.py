import zmq
import time

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.bind("tcp://localhost:5555")

#while True:
cnt = 0
for i in range(0, 100000):
    msg = f"Test message at {time.time()}\n"
    print("sending: ", msg)
    socket.send_string(msg)
    cnt += 1
    time.sleep(0.00000000001)
print(f"SENT {cnt} messages")
