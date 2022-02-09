#ifndef STREAM_RESPONSE_HPP
#define STREAM_RESPONSE_HPP

#include <vector>
#include  <iostream>
#include "socket.hpp"

using std::vector; 

struct StreamResponse {
    uint8_t* operator+(const size_t &len) {
        return buffer.data() + len;
    }
    void reserve(const size_t n) {
        buffer.reserve(n);
    } 
    vector<uint8_t> buffer;
};

class StreamReader {
/*
 * Reads binary stream of data into vectors
 */
public:
    StreamReader(const Socket fd_): fd(fd_) {  }
    StreamResponse read_stream() {
        size_t read_len = 0;
        size_t chunk = 2048;
        StreamResponse response;
	std::cerr << "read stream\n";
        do {
	  std::cerr << "Read len " << read_len << '\n';
            response.reserve(chunk);
            read_len += read(fd.fd(), response + read_len, chunk);
        } while(read_len == chunk);
        return response;
    }  
private:
    Socket fd;
};



#endif
