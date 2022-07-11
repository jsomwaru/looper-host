#include <iostream>
#include <algorithm>
#include <vector>

using std::vector;
const size_t FRAME_SIZE = 128;

struct Frame {
    uint8_t fin;
    size_t len;
    uint8_t mask[4];
    vector<uint8_t> payload;
    Frame(
        uint8_t finbit, 
        size_t _length, 
        uint8_t _mask[4]): fin(finbit), len(_length) { 
            std::copy(_mask, _mask+4, mask); 
    }
    Frame(const Frame& rhs): len(rhs.len), fin(rhs.fin) {
        std::copy(rhs.mask, rhs.mask+4, mask); 
        std::copy(rhs.payload.begin(), 
            rhs.payload.end(), std::back_inserter(payload)); 
    };
    ~Frame() {}
    Frame() {}
};

