This is a multithreaded proxy paxkage to distribute ZMQ pessages that come from a single source to multiple clients. 
The required packages to built this code are `zeromq`, `cppzmq` and `yaml-cpp`. 

To built the package:

```sh
cmake ./
make 
```

The code reads the `config.yaml` to extract the urls. The first ip and port in the `config.yaml` is the source address of the incoming message whereas the other variable number of urls are the receiving clients. 

There is a sender and a receiver code in python for testing purposes. The `sender.py` sends the messages to the proxy whereas the `receiver.py` receives the messages from the proxy. It is possible to run multiple instances of the `receiver.py` to increase the receiving client number.

The `pixi.toml` is currently configured for a sender and two receivers. Note that the messages between proxy and the clients are encrypted so one needs to create appropriate environment variables and export them while running the proxy and the clients. 

To test sending message to the proxy simply do 
```sh
pixi run sender
```

To test the receiving clients:

```sh
pixi run receiver1
```

and/or

```sh
pixi run receiver2
```

Note that the urls in the `config.yaml` and in the `pixi.toml` have to be the same. 


