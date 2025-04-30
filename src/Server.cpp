#include "Server.hpp"

Server::Server(const int port, const std::string& pass) : _port(port), _pass(pass) {
    memset(&_servAddress, 0, sizeof(_servAddress));
    _servAddress.sin_family = AF_INET;
    _servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    _servAddress.sin_port = htons(_port);

    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0) {
        std::cerr << "error: socket()" << std::endl;
        // do something for exit bcs idk
    }

    if (bind(_serverFd, (struct sockaddr*)&_servAddress, sizeof(_servAddress)) < 0) {
        std::cerr << "error: bind()" << std::endl;
        // do something for exit bcs idk
    }

    if (listen(_serverFd, SOMAXCONN) < 0) {
        std::cerr << "error: listen()";
        // do something for exit bcs idk
    }
}

Server::~Server() {
    std::cout << "Server was destroyed :(" << std::endl;
}
