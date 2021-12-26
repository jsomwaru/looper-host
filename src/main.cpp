#include "socket.hpp"
#include "protocol.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

#define DEFAULT_PORT 9600

using json = nlohmann::json;

void startmsg(int port) {
    std::cout << "Listening on " << port << std::endl;
}


int main (int argc, char **argv) {
    int port = DEFAULT_PORT;
    Socket sock = mksocket(port); 
    startmsg(port);
    while (true) {
        Socket fd = acceptor(sock);
        char *raw = protocol::readMsg(fd);        
        auto data = json::parse(raw);
        std::cout << data << std::endl;
    } 
}   
