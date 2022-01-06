#include "client_handler.hpp"
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;

namespace client {

    int client_handler(Socket &sock) {
        while(true) {
            std::string msg = protocol::readMsg(sock);
            auto req = protocol::parse_json(msg);
            protocol::json_handler(req); 
        }
    }

};
