#include "socket.hpp"
#include "protocol.hpp"
#include "client_handler.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

#define DEFAULT_PORT 9600

using json = nlohmann::json;

void startmsg(int port) {
    std::cout << "Listening on " << port << std::endl;
}

//#define print(msg) std::cout << msg << std::endl;

int main (int argc, char **argv) {
    int port = DEFAULT_PORT;
    Socket sock = mksocket(port); 
    std::vector<std::thread> thread_manager;
    startmsg(port);
    while (true) {
        Socket fd = acceptor(sock);
        std::string raw = protocol::readMsg(fd);
        if(!protocol::upgrade_connection(fd, raw)) {
            thread_manager.emplace_back(client::client_handler, std::ref(fd));
        }
        else {
            std::cerr << strerror(errno) << '\n';
        }
    }
    return 0;
}
