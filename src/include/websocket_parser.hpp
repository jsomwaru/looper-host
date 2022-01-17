#ifndef WEBSOCKET_PARSER_HPP
#define WEBSOCKET_PSRSER_HPP
#include <string>

using std::string;

class WebSocketParser {
public:
    WebSocketParser() {}
    WebSocketParser() {} 
    ~WebSocketParser() {}
    WebsocketParser(const WebSocketParser&) {}
    WebSocketParser& operator=(const WebSocketParser&);

    std::string getPayload() const;

    void encode();
    void decode();

private:
    string payload;


}

#endif
