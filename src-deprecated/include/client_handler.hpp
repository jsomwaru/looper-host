#include "websocket.hpp"
#include "protocol.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace client {
    json client_handler(WebSocket &sock);
};
