#include<zmq.hpp>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<yaml-cpp/yaml.h>
#include <ranges>
#include<iostream>

struct Node 
{
    std::string ip_addr;
    int port;
};

const char* server_pub;
const char* server_sec;

std::mutex buffer_mutex;
std::condition_variable cv;
std::queue<zmq::message_t> messages;



std::vector<Node> extract_ip()
{
    YAML::Node config = YAML::LoadFile("config.yaml");
    std::vector<Node> nodes;
    
    for (const auto& node : config["nodes"])
    {
        nodes.push_back({node["ip"].as<std::string>(), node["port"].as<int>()});
    }

    for (const auto& node : nodes)
    std::cout<<node.ip_addr<<" "<<node.port<<std::endl;

    return nodes;
}

void receive(zmq::context_t& context, const std::vector<Node>& nodes)
{
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.bind("tcp://" + nodes[0].ip_addr + ":" + std::to_string(nodes[0].port));
    
    while (true)
    {
        zmq::message_t msg;
        receiver.recv(msg, zmq::recv_flags::none);
       
        {   //lock the mutex until pushind the new message to the queue
            std::lock_guard<std::mutex> lock(buffer_mutex);
            
            std::string recv_msg((char*)msg.data(), msg.size());
            std::cerr<<"Received: "<< recv_msg<<std::endl;
            
            messages.emplace(std::move(msg)); // zmq::message_t does not have copy constructor?
        } //unlock mutex in the end of the scope
        cv.notify_one();

    }

}

void distribute(zmq::context_t& context, const std::vector<Node>& nodes)
{
    //create sockets to send the message through this proxy
    std::vector<zmq::socket_t> senders;
    for (const auto& node : nodes | std::views::drop(1))
    {
        zmq::socket_t sender(context, ZMQ_PUSH);
        sender.set(zmq::sockopt::curve_server, 1);
        sender.set(zmq::sockopt::curve_publickey, server_pub);
        sender.set(zmq::sockopt::curve_secretkey, server_sec);

        sender.connect("tcp://" + node.ip_addr + ":" + std::to_string(node.port));
        senders.push_back(std::move(sender));
    }

    while(true)
    {   
        // lock the mutex and preserve locking until the message buffer is not empty
        std::unique_lock<std::mutex> lock(buffer_mutex);
        cv.wait(lock, []{return !messages.empty(); }); 

        // get the message from the buffer queue and pop the buffer queue
        zmq::message_t msg = std::move(messages.front()); // zmq::message_t does not have copy constructor?
        messages.pop();
        lock.unlock(); // unlock the mutex so that the reader can continue working
      
        // sending message to socket might modify ownership? so just copy it into another instance 
        for (auto& sender : senders)
        { 
            zmq::message_t msg_copy;
            msg_copy.copy(msg); 
            sender.send(msg_copy, zmq::send_flags::none);
        }
       
    }

}

int main()
{
    server_pub = std::getenv("SERVER_PUBLIC_KEY");
    server_sec = std::getenv("SERVER_SECRET_KEY");
    std::cout<<"public key: "<<server_pub<<std::endl;
    std::cout<<"private key: "<<server_pub<<std::endl;

    std::vector<Node> nodes = extract_ip();

    zmq::context_t context(1); //1 IO thread
    std::thread th1(receive, std::ref(context), std::cref(nodes));
    std::thread th2(distribute, std::ref(context), std::cref(nodes));

    th1.join(); 
    th2.join();
 
    return 0;
}

