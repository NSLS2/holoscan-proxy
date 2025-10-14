import zmq
import time
import string
import random

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.bind("tcp://localhost:5555")

def generate_random_string_mb(size_mb):
    """
    Generates a random string of a specified size in megabytes.
    """
    # Calculate the number of characters needed for the desired size
    # Assuming 1 byte per character for simplicity (ASCII)
    num_chars = int(size_mb * 1024 * 1024) 
    
    # Define the character set to choose from
    characters = string.ascii_letters + string.digits + string.punctuation

    # Generate the random string
    random_string = ''.join(random.choice(characters) for _ in range(num_chars))
    return random_string

# Generate a 1MB random string
one_mb_string = generate_random_string_mb(1)

#while True:
cnt = 0
for i in range(0, 10000):
    msg = one_mb_string
    #msg = f"Test message at {time.time()}\n"
    #print("sending: ", msg)
    socket.send_string(msg)
    cnt += 1
    #time.sleep(0.0001)
print(f"SENT {cnt} messages")
while True:
    pass

