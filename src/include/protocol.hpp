#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "socket.hpp"
#include <vector>
#include "frame.hpp"

using std::vector;
using json = nlohmann::json;

typedef std::unordered_map<std::string,std::string> headerdict;

namespace protocol {
    std::string readMsg(int);
    std::string readMsg(const Socket&);
    headerdict parse_headers(const std::string&);
    int upgrade_connection(Socket&, std::string&);
    std::string upgrade_connection_payload(const std::string&);
    std::string decode_frame(std::string&);
    std::string encode_frame(const std::string&);
    vector<uint8_t> decode_frame_t (const vector<uint8_t> &data);
    
    template <typename T, typename A>
    Frame decode_frame_header(const vector<T,A>&buf) {
        uint8_t mask = 0x80;
        uint8_t optmask = 0x0f;
        uint8_t FIN = ((buf[0] & mask) >> 7);
        uint8_t OPT = (buf[0] & optmask);
        uint8_t MASKBIT = ((buf[1] & mask) >> 7);
        uint8_t ini_length = (buf[1] & 127);
        uint8_t offset = 0;
        uint64_t length = 0;
        if(ini_length == 126) {
            length = (buf[2] << 8) | buf[3];
            offset = 4;
        } else if (ini_length == 127) {
            memcpy(&length, &buf[2], sizeof(uint64_t));
            offset = 9;
        } else 
            offset = 2;
        uint8_t MASK[4];
        std::copy(&buf[offset], &buf[offset+4], MASK);
        return Frame(FIN, length, MASK);
    }
};

#endif
