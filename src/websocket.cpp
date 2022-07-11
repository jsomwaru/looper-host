#include "websocket.hpp"

void websocket_runtime_exception(const char *msg) {
    throw std::runtime_error(msg);
}

// Contructors
WebSocket::WebSocket() {
    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        websocket_runtime_exception("error allocating socket");
    }
}

// Methods
int WebSocket::websocket_listen(const int port) {
    int success;
    SocketAddress p = SocketAddress {port};
    if (bind(_fd, (sockaddr*)&p.address_, p.addrlen()) == -1 ) 
        websocket_runtime_exception("error binding");
    if ((success = listen(_fd, 3)) == -1) 
        websocket_runtime_exception("error listening");
    return success;
}

WebSocket WebSocket::websocket_accept() {
    SocketAddress addr;
    int newfd = accept(_fd, (sockaddr*)&addr.address_, (socklen_t*)addr.addrlen());
    return WebSocket(newfd, addr);
}

vector<uint8_t> WebSocket::socket_read(size_t chunk = 2048) {
    size_t chunk_size = chunk;
    int read_len;
    int offset = 0;
    vector<uint8_t> data(chunk_size);
    while((read_len = read(_fd, &(data.begin()+offset), chunk_size) != 0)) {
        offset += read_len;
        data.resize(data.size()+chunk_size);
    }
    data.shrink_to_fit();
    return data;
}

vector<uint8_t> WebSocket::websocket_read() {
    if (!_upgraded && !upgrade_connection()) {
        websocket_close();
        websocket_runtime_exception("Could not upgrade connection");
    }
    vector<uint8_t> data = socket_read(FRAME_SIZE);
    Frame f = get_frame_parameters(data);
}

size_t WebSocket::upgrade_connection() {
    vector<uint8_t> data = socket_read();
    std::string rawheaders(data.begin(), data.end());
    std::string payload = protocol::upgrade_connection_payload(rawheaders);
    size_t sent = socket_send((void*)payload.c_str());
    _upgraded = true;
    return sent;
}

size_t WebSocket::socket_send(void *buf) {
    unsigned char *payload = reinterpret_cast<unsigned char*>(buf);
    size_t len = sizeof(payload) / sizeof(*payload);
    return send(_fd, payload, len, 0);
}

Frame WebSocket::get_frame_parameters(const vector<uint8_t> &buf) {
    Frame a = protocol::decode_frame_buffer(buf);
    return a;
}