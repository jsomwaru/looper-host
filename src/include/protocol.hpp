#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
#include <stdlib.h>
#include <unistd.h>
#include "socket.hpp"

namespace protocol {
    char* readMsg(int);
    char* readMsg(Socket);
    int   upgrade_connection(Socket, char*);
    std::string testhash(std::string);
};

#endif
