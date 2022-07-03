#ifndef STREAM_RESPONSE_HPP
#define STREAM_RESPONSE_HPP

#include <vector>
#include <iostream>
#include <iterator>
#include <poll.h> 
#include "socket.hpp"

using std::vector; 

struct StreamResponse {
  uint8_t* operator+(const size_t &len) {
        return buffer.data() + len;
    }
  
  void resize(const size_t n) {
    buffer.resize(n);
  }

  void write_buffer(const uint8_t *data, const size_t len) {
      std::cerr << "writing\n";
      std::copy(data, data+len, std::back_inserter(buffer));
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
        pollfd pfd = fd.poller();
        size_t total = 0;
        do {
            poll(&pfd, 1, -1); 
       	    uint8_t data[chunk];
            if(pfd.revents & POLLIN)
                std::cerr << "socket is ready\n";
            std::cerr << "about to call read mf\n";
            read_len = read(fd.fd(), data, chunk);
            std::cerr << "Read len " << read_len << '\n';
            response.write_buffer(data, read_len);
            std::cerr << "Total " << response.buffer.size() << '\n';
        } while(read_len != 0);
        return response;
    }  
private:
    Socket fd;
};



#endif
