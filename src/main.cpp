#include "socket.hpp"
#include "protocol.hpp"
#include <iostream>
#include <string.h>


#define DEFAULT_PORT 9600

int main (int argc, char **argv) {
    int port = DEFAULT_PORT;
    Socket sock = mksocket(port);    
    while (true) {
        Socket fd = acceptor(sock);
        char *data = protocol::readMsg( fd );        
        
    } 
}   
