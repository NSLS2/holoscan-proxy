#include<zmq.hpp>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<iostream>


std::mutex buffer_mutex;
std::condition_variable cv;
std::queue<zmq::message_t> messages;

void receive(zmq::context_t& context)
{
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.bind("tcp://localhost:5555");

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

void distribute(zmq::context_t& context)
{
    //create sockets to send the message through this proxy
    zmq::socket_t sender1(context, ZMQ_PUSH);
    zmq::socket_t sender2(context, ZMQ_PUSH);

    sender1.connect("tcp://localhost:6001");
    sender2.connect("tcp://localhost:6002");

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
        zmq::message_t msg_copy;
        msg_copy.copy(msg); 
       
        sender1.send(msg, zmq::send_flags::none);
        sender2.send(msg_copy, zmq::send_flags::none);
       
    }

}

int main()
{
    zmq::context_t context(1); //1 IO thread
    std::thread th1(receive, std::ref(context));
    std::thread th2(distribute, std::ref(context));

    th1.join(); 
    th2.join();
 
    return 0;
}

