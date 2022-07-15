#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <exception>
#include <string>
#include "protocol.hpp"
#include "socket.hpp"
#include "frame.hpp"

using std::vector;

class WebSocket {
public:
    WebSocket();
    WebSocket(int fd): _fd(fd) { }
    WebSocket(int fd, SocketAddress addr): _fd(fd), _addr(addr) {}
    int websocket_listen(int);
    WebSocket websocket_accept();
    vector<uint8_t> websocket_read(size_t);
    vector<uint8_t> socket_read(size_t, size_t);
    size_t socket_send(void*);
    int websocket_close();
    inline int fd() const { return _fd; }
private:
    int _fd;
    SocketAddress _addr;
    bool _upgraded = false;

    int upgrade_connection();
    Frame get_frame_parameters(const vector<uint8_t>&);
};

#endif
