#include "client_handler.hpp"
#include "protocol.hpp"
//#include "logging.hpp"
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
        std::string msg = protocol::readMsg(sock);
        if (!(protocol::upgrade_connection(sock, msg) > 0)) {
            return -1;
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
            std::cerr << "decoding data\n";
            protocol::decode_frame(trackdata); 
        }
       // std::string recieved = "{\"type\": \"recieved\"}";
       // sock.send_(recieved);
        sock.close_();
        return 0;
    }
};
