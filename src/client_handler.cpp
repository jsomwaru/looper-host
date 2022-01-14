#include "client_handler.hpp"
#include "protocol.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

namespace client {
    
    struct ClientConnection {
        bool upgraded = false;
        bool block = false;
    };

    static std::unordered_map<std::string, client::ClientConnection> clients;

    int client_handler(Socket sock) {
        std::string ip(sock.client_ip());
        if(clients[ip].upgraded == false) {
            std::string msg = protocol::readMsg(sock);
            if (!(protocol::upgrade_connection(sock, msg) > 0)) {
                return -1;
             }
            clients[ip].upgraded = true;
        }

        std::string trackdata = protocol::readMsg(sock);
        if (trackdata.length() == 0) {
            sock.close_();
            return 0;
        }
        try {
            json track = json::parse(trackdata);
            std::cerr << track.dump() << '\n';
        } catch(...) {
            std::cerr << "Got not JSON\n";
        }
        std::string recieved = "{\"type\": \"recieved\"}";
        sock.send_(recieved);
        sock.close_();
        return 0;
    }
};
