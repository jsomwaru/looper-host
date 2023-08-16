#include <iostream>
#include <thread>
#include <future>
#include <algorithm>
#include <unordered_map>
#include "websocket.hpp"
#include "client_handler.hpp"

#define DEFAULT_PORT 9600

using json = nlohmann::json;
using std::vector;

void startmsg(int port) {
    std::cout << "Listening on " << port << std::endl;
}

struct ClientConnection {
    std::thread* exec;
    std::promise<bool> done;
    bool closed; 

    bool get_thread_status() {
        auto future = done.get_future();
        return future.get();
    }

    void cleanup() {
        exec->join();
        delete exec;
        exec = nullptr;
    }
};

int main (int argc, char **argv) {
    vector<ClientConnection> thread_manager;
    
    WebSocket sock = WebSocket();
    int listening = sock.websocket_listen(DEFAULT_PORT);
    if (listening == -1) {
        std::cerr << "error lisetning\n";
        exit(1);
    }
    startmsg(DEFAULT_PORT);
    while (true) {
        WebSocket conn = sock.websocket_accept();
        if (!conn.valid()) {
            std::cerr << "invalid socket\n";
            exit(1);
        }
        std::cerr << "client connected\n";
        client::client_handler(conn);
        /*
        std::promise<bool> done;

        std::thread *th = new std::thread([&conn, &done] {
            client::client_handler(conn);
            done.set_value(true);
        });
        thread_manager.emplace_back(ClientConnection{th, std::move(done), false});

        std::remove_if(thread_manager.begin(),
            thread_manager.end(), [] (ClientConnection &cli) {
                bool fut = cli.get_thread_status();
                if(fut == false) {
                    std::cout << "thread getting cleaned" << std::endl;
                }
                if (fut) {
                    cli.cleanup();
                    return true;
                }
                return false;
            }
        );
        */
    } 
    return 0;
}
