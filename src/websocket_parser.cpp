#include "websocket_parser.hpp"

uint8_t mask = 0x80;                                                               
uint8_t optmask = 0x0f;

WebSocketParser::WebSocketParser(const WebSocketParser& wsp) {
    _payload = wsp._payload;
}

WebSocketParser& operator=(const WebSocketParser& wsp) {
    _payload = wsp._payload;
    return this;
}

void WebSocketParser::encode() {

}

void WebSocketParser::decode() {                                           
    const uint8_t *data = reinterpret_cast<const uint8_t*>(_payload.c_str());               
    uint8_t MASK[4];                                                       
    uint8_t FIN = ((data[0] & mask) >> 7);                                             
    uint8_t OPT = (data[0] & optmask);                                                 
    uint8_t MASKBIT = ((data[1] & mask) >> 7);
    uint8_t length = (data[1] & 127);
    uint8_t encoded[length];
    uint8_t decoded[length+1];
    std::copy(&data[2], &data[6], MASK);                                               
    std::copy(&data[6], &data[_payload.length()], encoded);                                 
    _payload.clear();
	_payload.reserve(length);
	for (int i = 0; i < length; ++i) {                                                 
    	_payload += std::static_cast<char>(encoded[i] ^ MASK[i % 4]);
    }
}
