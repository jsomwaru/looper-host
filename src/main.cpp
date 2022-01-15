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

int main (int argc, char **argv) {
    int port = DEFAULT_PORT;
    Socket sock = mksocket(port); 
    std::vector<std::unique_ptr<std::thread>> thread_manager;
    startmsg(port);    
    while (true) {
        Socket fd = acceptor(sock);
        thread_manager.emplace_back(new std::thread(client::client_handler, fd));
        if (thread_manager.size() > 10) {
            for (auto &i: thread_manager) {
                i->join();
                i.reset();
            }
        }
    }
    return 0;
}
