#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "socket.hpp"

using json = nlohmann::json;

typedef std::unordered_map<std::string,std::string> headerdict;

namespace protocol {
    std::string readMsg(int);
    std::string readMsg(const Socket&);
    headerdict parse_headers(std::string&);
    int upgrade_connection(Socket&, std::string&);
    json parse_json(std::string&);
    int json_handler(const json&);
};

#endif
