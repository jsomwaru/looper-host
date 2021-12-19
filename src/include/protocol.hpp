#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
#include <stdlib.h>
#include <unistd.h>
#include "socket.hpp"

namespace protocol {
    char* readMsg(int);
    char* readMsg(Socket);
};

#endif
