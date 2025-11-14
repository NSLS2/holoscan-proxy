#include <any>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <ranges>
#include <string>
#include <thread>
#include <yaml-cpp/yaml.h>
#include <zmq.hpp>

#include "error_checker.h"
#include "socket.h"

const char *server_pub;
const char *server_sec;

std::mutex buffer_mutex;
std::condition_variable cv;
std::queue<zmq::message_t> message_buffer;
// int count=0;

std::vector<Node> extract_ip(const std::string &filepath) {

  std::ifstream file(filepath);
  if (!file) {
    throw std::runtime_error("Error: File '" + filepath + "' does not exist.");
  }

  YAML::Node config = YAML::LoadFile(filepath);
  std::vector<Node> nodes;

  YAML::Node sender = config["sender"];
  nodes.push_back({sender["ip"].as<std::string>(), sender["port"].as<int>()});

  std::cout << "sender: \n";
  std::cout << sender << std::endl;

  std::cout << "receivers: \n";
  for (const auto &receiver : config["receivers"]) {
    nodes.push_back({receiver["ip"].as<std::string>(),
                     receiver["port"].as<int>(), receiver["encrypt"].as<bool>(),
                     receiver["socket_type"].as<zmq::socket_type>()});
    std::cout << receiver << std::endl;
  }

  return nodes;
}

void receive(zmq::context_t &context, const std::vector<Node> &nodes) {
  zmq::socket_t receiver(context, ZMQ_PULL);

  std::any any_url = std::string("tcp://" + nodes[0].ip_addr + ":" +
                                 std::to_string(nodes[0].port));
  LOG_SOCKOUT_VOID("connect", any_url, [&receiver](const std::any &any_url) {
    return receiver.connect(std::any_cast<std::string>(any_url));
  });

  std::string url = std::any_cast<std::string>(any_url);

  while (true) {
    zmq::message_t msg;
    auto recv = LOG_SOCKOUT_BOOL("receive", url, [&receiver, &msg]() {
      return receiver.recv(msg, zmq::recv_flags::none);
    });
    // receiver.recv(msg, zmq::recv_flags::none);
    { // lock the mutex until pushind the new message to the queue
      std::lock_guard<std::mutex> lock(buffer_mutex);

      // uncomment for debug purposes
      // std::string recv_msg((char *)msg.data(), msg.size());
      // std::cerr << "Received: " << recv_msg << std::endl;

      message_buffer.emplace(
          std::move(msg)); // zmq::message_t does not have copy constructor?
      // std::cerr<<"Received  the message number : "<< count <<std::endl;
      // count +=1;
    } // unlock mutex in the end of the scope
    cv.notify_one();
  }
}

void distribute(zmq::context_t &context, const std::vector<Node> &nodes) {
  // create sockets to send the message through this proxy
  struct sockets {
    zmq::socket_t socket;
    std::string url;
  };

  std::cerr << "Started distribuing....\n";

  std::vector<sockets> senders;

  for (const auto &node : nodes | std::views::drop(1)) {
    // zmq::socket_t socket(context, ZMQ_PUSH);
    zmq::socket_t socket(context, node.socket_type);
    if (node.encrypt) {
      std::cerr << "encrypting the receiver" << std::endl;
      //LOG_SOCKOUT_VOID(
      //    "set", zmq::sockopt::curve_server, [&socket](const std::any &option) {
      //      return socket.set(
      //          std::any_cast<zmq::sockopt::curve_server_t>(option), 1);
      //    });

      socket.set(zmq::sockopt::curve_server, 1);

      //LOG_SOCKOUT_VOID(
      //    "set", zmq::sockopt::curve_publickey,
      //    [&socket](const std::any &option) {
      //      return socket.set(
      //          std::any_cast<zmq::sockopt::curve_publickey_t>(option),
      //          server_pub);
      //    });

      socket.set(zmq::sockopt::curve_publickey, server_pub);

      //LOG_SOCKOUT_VOID(
      //    "set", zmq::sockopt::curve_secretkey,
      //    [&socket](const std::any &option) {
      //      return socket.set(
      //          std::any_cast<zmq::sockopt::curve_secretkey_t>(option),
      //          server_sec);
      //    });

      socket.set(zmq::sockopt::curve_secretkey, server_sec);

    }
    std::any any_url =
        std::string("tcp://" + node.ip_addr + ":" + std::to_string(node.port));
    LOG_SOCKOUT_VOID("bind", any_url, [&](const std::any &any_url) {
      return socket.bind(std::any_cast<std::string>(any_url));
    });
    senders.push_back({std::move(socket), std::any_cast<std::string>(any_url)});
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(50));

  while (true) {
    // lock the mutex and preserve locking until the message buffer is not empty
    std::unique_lock<std::mutex> lock(buffer_mutex);
    cv.wait(lock, [] { return !message_buffer.empty(); });

    // get the message from the buffer queue and pop the buffer queue
    zmq::message_t msg = std::move(
        message_buffer
            .front()); // zmq::message_t does not have copy constructor?
    message_buffer.pop();
    lock.unlock(); // unlock the mutex so that the reader can continue working

    // sending message to socket might modify ownership? so just copy it into
    // another instance
    for (auto &sender : senders) {
      zmq::message_t msg_copy;
      msg_copy.copy(msg);

      // uncomment for debug purposes
      // std::string send_copy((char *)msg_copy.data(), msg_copy.size());
      // std::cerr << "Sending: " << send_copy << std::endl;

      /* //This Part is to make sure proxy send the messages but I am not sure
      if it is needed
      // Therefore, I am disabling it for now
      auto sent = LOG_SOCKOUT_BOOL(
          "send", sender.url, [&sender, &msg_copy]() -> std::optional<size_t> {
            // Try non-blocking send first

            auto result =
                sender.socket.send(msg_copy, zmq::send_flags::dontwait);
            if (result.has_value()) {
              return result;
            }

            // If send failed (would block), poll to wait for socket readiness
            zmq::pollitem_t items[] = {
                {static_cast<void *>(sender.socket), 0, ZMQ_POLLOUT, 0}};

            int rc = zmq::poll(items, 1, std::chrono::milliseconds(100));

            if (rc > 0 && items[0].revents & ZMQ_POLLOUT) {
              // Socket ready, try sending again
              return sender.socket.send(msg_copy, zmq::send_flags::dontwait);
            }

            // Failed even after poll
            // return std::nullopt;
            return std::nullopt;

          });
            */

      auto sent = LOG_SOCKOUT_BOOL("send", sender.url, [&sender, &msg_copy]() {
        return sender.socket.send(msg_copy, zmq::send_flags::dontwait);
      });
    }
  }
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    throw std::runtime_error("No directory for configuration was provided!!");
  }

  std::string filepath = argv[1];

  try {
    server_pub = std::getenv("SERVER_PUBLIC_KEY");
    server_sec = std::getenv("SERVER_SECRET_KEY");
    if (!server_pub) {
      std::cout << "Warning!!! SERVER_PUBLIC_KEY is not set, as expected."
                << std::endl;
    }
    if (!server_sec) {
      std::cout << "Warning!!! SERVER_SECRET_KEY is not set, as expected."
                << std::endl;
    }
  } catch (const std::runtime_error &e) {
    std::cerr << "Error while setting environment variables of the server!!!"
              << e.what() << std::endl;
    std::terminate();
  }

  // below is for debug purposes . delete immediately afterwords
  // std::cerr << "SERVER_PUBLIC_KEY: " << server_pub << std::endl;
  // std::cerr << "SERVER_SECRET_KEY: " << server_sec << std::endl;

  std::cerr << "**** HOLOSCAN PROXY LAUNCH WELCOME MESSAGE : WAITING FOR "
               "MESSAGES ****\n";

  std::vector<Node> nodes = extract_ip(filepath);

  zmq::context_t context(1); // 1 IO thread
  std::thread th1(receive, std::ref(context), std::cref(nodes));
  std::thread th2(distribute, std::ref(context), std::cref(nodes));

  th1.join();
  th2.join();

  return 0;
}
