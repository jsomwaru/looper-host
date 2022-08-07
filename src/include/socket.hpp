#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <poll.h>

using std::ostream; 

struct SocketAddress {
    SocketAddress() {}
    SocketAddress(int port) {
        address_.sin_family = AF_INET;
        address_.sin_addr.s_addr = htonl(INADDR_ANY);
        address_.sin_port = htons( port );
    }
    void print() const {
         printf("New connection  ip is : %s , port : %d\n" , inet_ntoa(address_.sin_addr) , ntohs(address_.sin_port));
    }
    SocketAddress(const SocketAddress&);
    inline const size_t addrlen() { return sizeof(address_); }
    SocketAddress& operator=(const SocketAddress&);
    sockaddr_in address_;
};

class Socket {
// Only AF_INET Connections  
public:    
    Socket(): opt(1), handle_(-1), port_() {}
    Socket(int);
    Socket(int, int);
    Socket(const Socket&);
    Socket(Socket&&);
    ~Socket() { } 
    Socket& operator=(const int&);
    Socket& operator=(const Socket&);
    
    int bind_();
    int listen_();
    Socket accept_();
    ssize_t send_(const std::string&);

    int fd() const { return handle_; } 
    void close_ () { close(handle_); }

    inline char* client_ip() const { return inet_ntoa(port_.address_.sin_addr); }

    pollfd poller();

    friend Socket acceptor(Socket&);
    friend Socket mksocket(Socket);
    friend ostream& operator<<(ostream&, const Socket&);
private:
    int opt;
    int handle_;
    SocketAddress port_;
};

extern Socket mksocket(int);
extern Socket acceptor(Socket&);
extern std::ostream& operator<<(ostream&, const Socket&);

#endif // SOCKET_HPP
