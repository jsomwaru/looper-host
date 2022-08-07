#ifndef FRAME_HPP
#define FRAME_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>

using std::vector;
using std::array;
const size_t FRAME_SIZE = 32;


struct payload_length {
    enum class PAYLOAD_SIZE{LARGE, MID, SMALL} size;
    union {
        uint64_t large_payload;
        uint16_t mid_payload;
        uint8_t small_payload;
    };
};

struct Frame {
public:
    uint8_t fin;
    size_t len;
    array<uint8_t, 4> mask;
    vector<uint8_t> payload;

    Frame(): fin(0), len(0), payload(vector<uint8_t>()) {}

    Frame(
        uint8_t finbit, 
        size_t _length, 
        uint8_t _mask[4]): fin(finbit), len(_length) { 
            std::copy(_mask, _mask+4, std::begin(mask)); 
    }

    Frame(const vector<uint8_t> &buf) {
        fin = 0;
        len = buf.size();
        payload = buf;
    }

    void set_payload(const vector<uint8_t> &buf) { payload = buf; }
    
    vector<uint8_t> websocket_payload(uint8_t finish, uint8_t opt) {
        size_t header_size = 0;
        vector<uint8_t> length;
        set_length(length);
        vector<uint8_t> ws_payload(payload.size() + header_size);
        ws_payload[0] |= ((finish << 7) & 0x80); // set fin
        ws_payload[0] |= (opt & 0xf); // set opt 
        ws_payload[1] |= (0 & 0xf); // set mask to 0
        uint8_t rsv = 0;
        // Reasons I hate the websocket protocol
        // I am just too stupid to understand the consequenses of making a word aligned protocol
        for (int i = 0; i < length.size(); ++i) {
            rsv = length[i] & 1;
            ws_payload[i+1] |= (length[i] & 0x7f);
            ws_payload[i+2] |= ((rsv << 7) & 0x80);
        }
        std::copy(payload.begin(), 
            payload.end(), std::back_inserter(ws_payload));
        return ws_payload;
    }

    friend std::ostream& operator<<(std::ostream& out , const Frame& f) {
         out << "Length: " << f.len  << '\n'
             << "Finish: " << unsigned(f.fin) << '\n';
         return out;
    }

private:
    void set_length(vector<uint8_t> &length) {
        size_t size = length.size();
        if (size <= 126) {
            uint8_t payload_size = static_cast<uint8_t>(size);
            reserve_length(length, sizeof(uint8_t), payload_size);
        }
        else if (size > 126 && size <= 65536) {
            uint16_t payload_size = static_cast<uint16_t>(size);
            reserve_length(length, sizeof(uint16_t), payload_size);
        }
        else {
            uint64_t payload_size = static_cast<uint64_t>(size);
            reserve_length(length, sizeof(uint64_t), payload_size);
        }
    }

    template <typename T>
    void reserve_length(vector<uint8_t> &length, const size_t length_size, T payload_size) {
            length.reserve(length_size);
            uint8_t *tmp_length = 
                *reinterpret_cast<uint8_t(*)[length_size]>(&payload_size);
            length.insert(length.end(),tmp_length, tmp_length + length_size);
    }
};

#endif
