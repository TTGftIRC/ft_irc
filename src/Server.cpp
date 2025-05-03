#include "../inc/Server.hpp"

Server::Server(const int port, const std::string& pass) : _port(port), _pass(pass) {
    int yes = 1;
    memset(&_servAddress, 0, sizeof(_servAddress));
    _servAddress.sin_family = AF_INET;
    _servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    _servAddress.sin_port = htons(_port);

    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0) {
        std::cerr << "error: socket()" << std::endl;
        // do something for exit bcs idk
    }
    
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        std::cerr << "error: setsockopt()" << std::endl;
        // do something for exit bcs idk
    }

    if (bind(_serverFd, (sockaddr*)&_servAddress, sizeof(_servAddress)) < 0) {
        std::cerr << "error: bind()" << std::endl;
        // do something for exit bcs idk
    }

    if (listen(_serverFd, SOMAXCONN) < 0) {
        std::cerr << "error: listen()" << std::endl;
        // do something for exit bcs idk
    }

    _setNonBlock(_serverFd);

    _serverPollFd.fd = _serverFd;
    _serverPollFd.events = POLL_IN;
    _serverPollFd.revents = 0;
    _pollFds.push_back(_serverPollFd);
}

Server::~Server() {
    std::cout << "Server was destroyed :(" << std::endl;
}

int Server::_setNonBlock(int sockFd) {
    if (fcntl(sockFd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "error: fcntl(set_nonblock)" << std::endl;
    }
    return sockFd;
}

void Server::run() {
    for (;;) {
        int pollC = poll(&_pollFds[0], _pollFds.size(), -1);
        if (pollC < 0) {
            std::cerr << "error: poll()" << std::endl;
            break;
        } else if (errno == EINTR) {
            std::cout << "error: poll() was interupted, continuing" << std::endl;
            continue;
        }
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].fd == _serverFd) {
                if (_pollFds[i].revents & POLLIN) {
                    //new client, Here is I will have to make a client obj and store it somewhere, I am not sure that it should be outside of the server obj
                    sockaddr_in clientAddress;
                    socklen_t clientLen = sizeof(clientAddress);

                    int clientFd = accept(_serverFd, (sockaddr*)&clientAddress, &clientLen);
                    if (clientFd < 0) {
                        std::cerr << "error: accept()" << std::endl;
                    } else {
                        if (_setNonBlock(clientFd) < 0) {
                            break;
                        }
                        pollfd clientPollFd;
                        clientPollFd.fd = clientFd;
                        clientPollFd.events = POLLIN;
                        clientPollFd.revents = 0;
                        _pollFds.push_back(clientPollFd);

                        std::cout << "New client arrived to server, fd: " << clientPollFd.fd << std::endl;

                        //from here the client logic goes
                    }
                } //here do the error check for revents on server socket (listener)
            } else {
                int currFd = _pollFds[i].fd;

                if (_pollFds[i].revents & POLLIN) {
                    char buf[1024];

                    ssize_t bytes = recv(currFd, buf, 1024 - 1, 0);

                    if (bytes > 0) {
                        std::cout << "Recieved from client fd: " << currFd << ": " << buf;
                        //handling the data which recieved from the client gabba gabba
                    } else if (bytes <= 0) {
                        // for now errors and disconnect going to be poorly handled
                        std::cout << "Client disconnected or had a error, client fd: " << currFd << std::endl;
                        close(currFd);
                        _pollFds.erase(_pollFds.begin() + i);
                        --i;
                    }
                    memset(buf, 0, 1024);
                }
            }
        }
    }

    for (size_t i = 1; i < _pollFds.size(); ++i) {
        close(_pollFds[i].fd);
    }

    close(_serverFd);
}