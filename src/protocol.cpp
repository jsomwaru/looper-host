#include <cryptlite/sha1.h>
#include "protocol.hpp"
#include <utility>
#include <algorithm>
#include <nlohmann/json.hpp>

using namespace nlohmann;

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
    
    std::string readMsg(int fd) {
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
            readn = read(fd, buf, chunk_sz);
            len += readn;
        } while(readn == chunk_sz);
        if(len == cap) {
           buf = (char*)realloc(buf, sizeof(char));
        }
        buf[len] = '\0';
        std::string newbuf(buf);
        delete [] buf;
        return newbuf;
    }

    std::string readMsg(const Socket &sock) {
        return readMsg(sock.fd());
    }

    int upgrade_connection(Socket &sock, std::string &headers) {
        headerdict parsed_headers = protocol::parse_headers(headers);
        std::string sentkey(parsed_headers["Sec-WebSocket-Key"]);
        std::string acceptkey = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        std::string socketkey = cryptlite::sha1::hash_base64(sentkey + acceptkey); 
        std::string upgrade   = "HTTP/1.1 101 Switching Protocols\r\n"
                                "Upgrade: websocket\r\n" 
                                "Connection: Upgrade\r\n";
        upgrade.append("Sec-WebSocket-Accept: " + socketkey + "\r\n\r\n");
        auto tmp = sock.send_(upgrade);
        return tmp;
    }

    int json_handler(const json& req) {
        std::string type = req["type"].get<std::string>();
        if (type == "new") {
            std::cout << "New track recieved";
        }
        return 0;
    }

    headerdict parse_headers(std::string &rawheaders) {
        std::istringstream iss(rawheaders);
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
    
    json parse_json(std::string& blob) {
        json j = json::parse(blob);
        return j;
    }
}
