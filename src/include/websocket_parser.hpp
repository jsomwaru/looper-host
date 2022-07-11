#ifndef WEBSOCKET_PARSER_HPP
#define WEBSOCKET_PSRSER_HPP
#include <string>

using std::string;

class WebSocketParser {
public:
    WebSocketParser() {}
    WebSocketParser(const string &payload): _payload(payload) {} 
    ~WebSocketParser() {}
    WebSocketParser(const WebSocketParser&) {}
    WebSocketParser& operator=(const WebSocketParser&);

    inline string payload() const { return _payload; }

    void encode();

private:
    string _payload;
};

#endif
