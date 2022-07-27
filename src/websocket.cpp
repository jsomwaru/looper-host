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

vector<uint8_t> WebSocket::socket_read(size_t bytes, size_t chunk) {
    int read_len;
    int offset = 0;
    std::vector<uint8_t> data(chunk);
    while((read_len = recv(_fd, &data[0]+offset, chunk, 0) != 0)) {
        offset += read_len;
        if (bytes != 0 && offset >= bytes)
            break;
        data.resize(offset+chunk);
    }
    data.shrink_to_fit();
    return data;
}

WebSocketCodec WebSocket::websocket_read(size_t bytes) {
    if (!_upgraded && !upgrade_connection()) {
        websocket_close();
        websocket_runtime_exception("Could not upgrade connection");
    }
    Frame f;
    vector<uint8_t> data;
    WebSocketCodec frames;
    while(f.fin == 0) {
        vector<uint8_t> fr = socket_read(FRAME_SIZE);
        f = get_frame_parameters(fr);
        vector<uint8_t> chunk = socket_read(f.len);
        // decode payload w mask
        data.insert(data.end(), chunk.begin(), chunk.end());
        if(bytes != 0 && data.size() >= bytes) {
            data.resize(bytes);
            break;
        }
        frames.append_frame(f);
    }
    return frames;
}

int WebSocket::upgrade_connection() {
    vector<uint8_t> data = socket_read();
    std::string rawheaders(data.begin(), data.end());
    std::string payload = protocol::upgrade_connection_payload(rawheaders);
    size_t sent = socket_send(payload.c_str(), payload.size()); 
    _upgraded = sent > 0 ? true : false;
    return (int)sent;
}

template <typename T>
size_t WebSocket::socket_send(const T *buf, size_t size) {
    const unsigned char *payload = reinterpret_cast<const unsigned char*>(buf);
    return send(_fd, payload, size, 0);
}

int WebSocket::websocket_close() {
    close(_fd);
    return 0;
}

Frame WebSocket::get_frame_parameters(const vector<uint8_t> &buf) {
    return protocol::decode_frame_header(buf);
}