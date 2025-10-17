#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <zmq.hpp>

struct Node {
  std::string ip_addr;
  int port;
  bool encrypt;
  zmq::socket_type socket_type;
};

template <> struct YAML::convert<zmq::socket_type> {

  static bool decode(const Node &node, zmq::socket_type &socketType) {
    static const std::unordered_map<std::string, zmq::socket_type> mapping = {
        {"PULL", zmq::socket_type::pull},
        {"PUSH", zmq::socket_type::push},
        {"PUB", zmq::socket_type::pub},
        {"SUB", zmq::socket_type::sub},
        {"REQ", zmq::socket_type::req},
        {"REP", zmq::socket_type::rep},
        {"DEALER", zmq::socket_type::dealer},
        {"ROUTER", zmq::socket_type::router},
    };
    std::string value = node.as<std::string>();
    auto it = mapping.find(value);
    if (it == mapping.end()) {
      throw std::runtime_error("Unknown socket type!" + value);
    }

    socketType = it->second;
    return true;
  };
};

#endif
