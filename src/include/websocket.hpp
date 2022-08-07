#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <exception>
#include <string>
#include "protocol.hpp"
#include "frame.hpp"
#include "websocket_codec.hpp"

// Move all socket_xxx methods to socket class and use as base class

using std::vector;

class WebSocket {
public:
    WebSocket();
    WebSocket(int fd): _fd(fd) { }
    WebSocket(int fd, SocketAddress addr): _fd(fd), _addr(addr) {}
    int websocket_listen(int);
    WebSocket websocket_accept();
    int websocket_close();
    WebSocketCodec websocket_read(size_t bytes=0);
    inline int fd() const { return _fd; }
    inline bool valid() const { return _fd != -1; }
    
    // Refactor to socket class
    template <typename T>
    size_t socket_send(const T *buf, size_t size);
    vector<uint8_t> socket_read(const size_t bytes=0, const size_t chunk=2048);
private:
    int _fd;
    SocketAddress _addr;
    bool _upgraded = false;

    int upgrade_connection();
    Frame get_frame_parameters(const vector<uint8_t>&);
};

#endif
