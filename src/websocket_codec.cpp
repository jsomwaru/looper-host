#include "websocket_codec.hpp"

void WebSocketCodec::append_frame(const Frame& frame) {
   frames.push_back(frame);
}

void WebSocketCodec::decode() { 
    for(auto &frame: frames) { 
        for(size_t i = 0; i < frame.payload.size(); ++i) {
            frame.payload[i] = frame.payload[i] ^ frame.mask[i % 4];
        }
    }
}

vector<uint8_t> WebSocketCodec::get_payload() {
    vector<uint8_t> data;
    for(auto &frame: frames) {
        data.insert(data.end(), frame.payload.begin(), frame.payload.end());
    }
    return data;
}

void WebSocketCodec::encode(const vector<uint8_t> &buf) { 
    /*
    Takes random data and makes a builds a frame that can be
    TODO: find ideal length to make frames
    */
   Frame f = Frame(buf);
   append_frame(f);
}

vector<uint8_t> WebSocketCodec::get_websocket_payload() {
    vector<uint8_t> payload;
    for (auto &frame: frames) {
        auto buf = frame.websocket_payload(0, 0x2);
        payload.insert(payload.end(), buf.begin(), buf.end());
    }
    return payload;
}