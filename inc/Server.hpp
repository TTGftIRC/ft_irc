#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

class Server {
    private:
        const int _port;
        const std::string _pass;
        sockaddr_in _servAddress;
        int _serverFd;
    public:
        Server(const int port, const std::string& pass);
        ~Server();
};

#endif