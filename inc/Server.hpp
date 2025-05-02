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
#include <errno.h> // In subject not specifed specificaly about the errno but it is not a function, it is macro

class Server {
    private:
        const int _port;
        const std::string _pass;
        sockaddr_in _servAddress;
        int _serverFd;
        pollfd _serverPollFd;
        std::vector<struct pollfd> _pollFds;
    private:
        int _setNonBlock(int sockFd);
    public:
        Server(const int port, const std::string& pass);
        void run();
        ~Server();
};

#endif