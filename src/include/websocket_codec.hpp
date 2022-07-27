#ifndef WEBSOCKET_PARSER_HPP
#define WEBSOCKET_PARSER_HPP
#include <vector>
#include "frame.hpp"

using std::vector;

class WebSocketCodec {
/*
Using this class to assemble 
WebSocketFrames into the data we all want
*/
public:
    WebSocketCodec() : frames(vector<Frame>()) {}
    void decode();
    void encode(const vector<uint8_t>&);
    void append_frame(const Frame&);
    vector<uint8_t> get_payload();
    vector<uint8_t> get_websocket_payload();
private:
    vector<Frame> frames;
   
};

#endif
