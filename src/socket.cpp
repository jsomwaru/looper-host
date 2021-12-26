#include "socket.hpp"
#include <stdexcept>

Socket::Socket(int handle) {
    if(handle<=0) {
        throw std::runtime_error("invaild socket");
    } 
    handle_ = handle;
    port_ = SocketAddress();
}   


Socket::Socket(int handle, int port) {
    if (handle<=0)
        throw std::runtime_error("invalid socket");
    handle_ = handle; 
    port_ = SocketAddress(port);
    opt = 1;
    if (setsockopt(handle_, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t)sizeof(opt))) {
        throw std::runtime_error("Socket failed setsockopt");
    } 
}

Socket& Socket::operator=(const int &sockfd) {
    handle_ = sockfd;
    port_ = SocketAddress();
    return *this;
}

SocketAddress& SocketAddress::operator=(const SocketAddress& rhs) {
    if (this == &rhs) 
        return *this;
    this->address_ = rhs.address_;
    return *this;
}

SocketAddress::SocketAddress(const SocketAddress& rhs) {
   address_ = rhs.address_;
}   

int Socket::bind_() {
    return bind(handle_, (sockaddr*)&port_.address_, port_.addrlen()); 
}

int Socket::listen_() {
    return listen(handle_, 3);
}

Socket Socket::accept_() {
    Socket tmp;
    int len = tmp.port_.addrlen();
    int fd = accept(handle_, (sockaddr*)&tmp.port_.address_, (socklen_t*)&len);
    tmp.handle_ = fd;
    return tmp;
}

Socket acceptor(Socket &sock) {
    return sock.accept_();
}

Socket mksocket(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    Socket tmp(fd, port);
    tmp.bind_();
    tmp.listen_();
    return tmp;
}

ostream& operator<<(ostream &os, const Socket &sock) {
    return os << sock.handle_ << std::endl;
} 
