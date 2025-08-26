#include "../inc/Bot.hpp"

Bot::SuperException::SuperException(const std::string &err) : msg(err) { }

Bot::SuperException::~SuperException() throw() {}

const char* Bot::SuperException::what() const throw() { return msg.c_str(); }

Bot::Bot(int port, std::string pass) : _port(port), _pass(pass), _socket(-1) {

}

Bot::~Bot() {}

void Bot::sendMessage(const std::string& msg) {
    std::string out = msg + "\r\n";
    send(_socket, out.c_str(), out.size(), 0);
}

bool Bot::loginBot() {
    sendMessage("PASS pass");
    sendMessage("NICK Bot42");
    sendMessage("USER bot42 0 * :");
}

bool Bot::initBot() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent* server = gethostbyname("127.0.0.1");
    if (!server) {
        throw SuperException("No such host");
        return false;
    }
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(_port);
    if (connect(_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        throw SuperException("Unable to connect");
        return false;
    }
    return true;
}

void Bot::loopBot() {
    char buf[MAX_SIZE];

    while (!sig_recieved) {
        
    }
}