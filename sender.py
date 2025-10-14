import zmq
import time

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.bind("tcp://localhost:5555")

#while True:
cnt = 0
for i in range(0, 1000):
    msg = f"Test message at {time.time()}\n"
    print("sending: ", msg)
    socket.send_string(msg)
    cnt += 1
    #time.sleep(0.01)
print(f"SENT {cnt} messages")
