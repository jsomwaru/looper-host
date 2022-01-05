#include <cryptlite/sha1.h>
#include "protocol.hpp"
#include <unordered_map>
#include <utility>
#include <algorithm>

typedef std::unordered_map<std::string,std::string> headerdict;


void printdict(const headerdict &dict) {
    for (const auto &pair: dict ) {
        std::cerr << pair.first << ": " << pair.second << "\n";
    }
}

std::string rtrim(std::string &s) {
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}


namespace protocol {

    char* readMsg(int fd) {
        int chunk_sz = 2048;
        int len = 0;
        int cap = chunk_sz; 
        char *buf = (char*)malloc(chunk_sz*sizeof(char));
        int readn;
        do {
            if(len == cap) {
                buf = (char*)realloc(buf, chunk_sz*sizeof(char));
                cap += chunk_sz;
            }
            readn = recv(fd, buf, sizeof(buf), 0);
            len += readn;
        } while(readn == chunk_sz);
        if(len == cap) {
           buf = (char*)realloc(buf, sizeof(char));
        }
        buf[len] = '\0';
        return buf;
    }

    char* readMsg(const Socket &sock) {
        return readMsg(sock.fd());
    }

    headerdict parse_headers(char*);

    int upgrade_connection(Socket &sock, char *headers) {
        headerdict parsed_headers = protocol::parse_headers(headers);
        std::string sentkey(parsed_headers["Sec-WebSocket-Key"]);
        std::string acceptkey = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        std::string socketkey = cryptlite::sha1::hash_base64(sentkey + acceptkey); 
        std::string upgrade   = "HTTP/1.1 101 Switching Protocols\r\n"
                                "Upgrade: websocket\r\n" 
                                "Connection: Upgrade\r\n";
        upgrade.append("Sec-WebSocket-Accept: " + socketkey + "\r\n\r\n");
        auto tmp = sock.send_(upgrade);
        std::cerr << "Sent " << tmp << '\n';
        return tmp;
    }

    headerdict parse_headers(char *rawheaders) {
        std::string data(rawheaders);
        std::istringstream iss(data);
        std::string line;
        headerdict headers;
        while(getline(iss, line)) {
            int loc = line.find(':');
            if (loc != -1) {
                std::string header = line.substr(0, loc);
                // +2 for :\s 
                std::string value  = line.substr(loc+2, line.length()-1);
                std::string trimvalue = rtrim(value);
                headers.insert(std::make_pair(header, trimvalue));
            }
        } 
        return headers;
    }

    std::string testhash(std::string tmp) {
        return cryptlite::sha1::hash_hex(tmp);
    }
};
