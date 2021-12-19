#include "protocol.hpp"


namespace protocol {
    char* readMsg(int fd) {
        int chunk_sz = 2048;
        int len = 0;
        int cap = chunk_sz; 
        char *buf = (char*)malloc(chunk_sz*sizeof(char));
        int readn;
        do {
            if(len == cap) {
                buf = (char*)realloc(buf, chunk_sz*sizeof(char));
                cap += chunk_sz;
            }
            readn = read(fd, buf, chunk_sz);
            len += readn;
        } while(readn == chunk_sz);
        printf("%s", buf);  
        if(len == cap) {
            buf = (char*)realloc(buf, sizeof(char));
        }
        std::cerr << "Read " << len  << "Bytes\n";    
        buf[len] = '\0';
        return buf;
    }

    char* readMsg(Socket sock) {
        return readMsg(sock.fd());
    } 
};  
