#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "socket.hpp"
#include <vector>

using std::vector;
using json = nlohmann::json;

typedef std::unordered_map<std::string,std::string> headerdict;

namespace protocol {
    std::string readMsg(int);
    std::string readMsg(const Socket&);
    headerdict parse_headers(std::string&);
    int upgrade_connection(Socket&, std::string&);
    std::string decode_frame(std::string&);
    std::string encode_frame(const std::string&);
    vector<uint8_t> decode_frame_t (const vector<uint8_t> &data);
};

#endif
