#include "client_handler.hpp"
#include "protocol.hpp"
#include "websocket_codec.hpp"
//#include "logging.hpp"
#include "stream_response.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;
using std::vector;

namespace client {
    
    json client_handler(WebSocket &sock) {
        WebSocketCodec buf = sock.websocket_read();
        return json();
    }
};
