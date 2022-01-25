#include <cryptlite/sha1.h>
#include <cryptlite/base64.h>
#include <boost/beast/core/detail/base64.hpp>
//#include <boost/uuid/sha1.hpp>
#include "protocol.hpp"
#include <utility>
#include <algorithm>
#include <iterator>
#include <nlohmann/json.hpp>

using namespace nlohmann;

using namespace cryptlite;


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

void debug_msg(const std::string &msg) {
    std::cerr << msg 
        << " Length "
        << msg.size()
        << '\n';
}


namespace protocol {
    namespace util {
        std::string b64_encode(const std::string& data) {
            std::string dest;
            dest.resize(boost::beast::detail::base64::encoded_size(data.size()));            
            boost::beast::detail::base64::encode(&dest, data.c_str(), data.size());
            return dest;
        }
    };
    
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
            readn = read(fd, buf+len, chunk_sz);
            len += readn;
        } while(readn == chunk_sz);
        if(len == cap) {
           buf = (char*)realloc(buf, sizeof(char));
        }
        buf[len] = '\0';
        std::string newbuf(buf);
        debug_msg(newbuf);
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

    std::string decode_frame(std::string &raw) {
        const uint8_t *data = reinterpret_cast<const uint8_t*>(raw.c_str());
        uint8_t mask = 0x80;
        uint8_t optmask = 0x0f;
        uint8_t FIN = ((data[0] & mask) >> 7);
        uint8_t OPT = (data[0] & optmask);
        uint8_t MASKBIT = ((data[1] & mask) >> 7);
        uint64_t length = (data[1] & 127);
        uint8_t offset = 0;
        if (length == 126) {
            length = ((uint16_t) data[2] << 8) | data[3];
            ++offset;
        } else if (length > 126) {
            for(int i = 2; i > 10; i+=2 ) {
                length |= ((uint64_t) data[i] << 8) | data[i+1];
                if(i < 8) length = length << 16;
            }
        }
        uint8_t MASK[4];
        uint8_t encoded[length];
        uint8_t decoded[length+1];    
        std::copy(&data[2], &data[6], MASK);
        std::copy(&data[6], &data[raw.length()], encoded); 
        for (int i = 0; i < length; ++i) {
            decoded[i] = (encoded[i] ^ MASK[i % 4]);
        }
        decoded[length] = '\0';
        std::string decodeddata((char*)decoded);
        return decodeddata;
    }

    std::string encode_frame(const std::string &raw) {
        std::string frame;
        uint8_t payload_length(raw.length());
        char headers[2] = {0};
        //fin
        headers[0] |= 0xf;
        //opcode
        headers[0] |= 0x2;
        //mask
        headers[0] |= 0x1;
        headers[1] = (payload_length << 1);
        std::copy(headers, headers+2, std::back_inserter(frame));
        std::copy(raw.begin(), raw.end(), std::back_inserter(frame));
        return frame;
    }

};
