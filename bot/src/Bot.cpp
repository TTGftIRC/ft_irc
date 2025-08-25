#include "../inc/Bot.hpp"

Bot::Bot(int port, std::string pass) : _port(port), _pass(pass), _socket(-1) {

}

Bot::~Bot() {}

bool Bot::initBot() {
    std::string nick = "Bot42";
    std::string user = "superbot";
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent* server = gethostbyname("127.0.0.1");
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(_port);
    connect(_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

void Bot::loopBot() {
    
}