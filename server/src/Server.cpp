#include "../inc/Server.hpp"
#include <cstring>

Server::Server() {}

void Server::createSocket()
{
    _listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listening_socket == -1)
    {
        std::cerr << "Socket couldn't be created" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::initAdress()
{
    sockaddr_in serverAdress = {};

    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(8080);
    serverAdress.sin_addr.s_addr = INADDR_ANY; // setup serv adress

    // bind port w server
    if (bind(_listening_socket, reinterpret_cast<sockaddr *>(&serverAdress), sizeof(serverAdress)) == -1)
    {
        std::cerr << "Error: bind has failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::startListen()
{
    if (listen(_listening_socket, 5) == -1)
    {
        std::cerr << "Error: listen has failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    // replc pot 8080 w all ports
    std::cout << "Server is now listening on port 8080...." << std::endl;
}

void Server::runPoll()
{
    pollfd server_fd;
    server_fd.fd = _listening_socket;
    server_fd.events = POLLIN;
    _poll_fds.push_back(server_fd); // first elem of the pollfd will be the server which will be waiting for new events
    while (true)
    {
        if (poll(_poll_fds.data(), _poll_fds.size(), -1) == -1)
        {
            std::cerr << "Error: poll has failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (_poll_fds[0].revents & POLLIN)
        {
            if (_poll_fds[0].fd == _listening_socket)
            {
                // handle new client conexions
                socklen_t addr_len = sizeof(sockaddr_in);
                int new_socket = accept(_poll_fds[0].fd, NULL, &addr_len);
                if (new_socket == -1)
                {
                    std::cerr << "Error: accept has failed" << std::endl;
                    exit(EXIT_FAILURE);
                }
                pollfd new_conexion;
                new_conexion.fd = new_socket;
                new_conexion.events = POLLIN;
                _poll_fds.push_back(new_conexion);
            }
        }
        for (size_t i = 0; i < _poll_fds.size(); i++)
        {
            if (_poll_fds[i].revents & POLLIN)
            {
                // handle client/sock data
                char buffer[1024] = {0};
                size_t bytes_read = recv(_poll_fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytes_read > 0)
                {
                    std::cout << buffer << "\n";
                }
                else if (bytes_read == 0)
                {
                    std::cout << "Client has been disconnected !" << std::endl;
                    close(_poll_fds[i].fd);
                    _poll_fds.erase(_poll_fds.begin() + i);
                    --i;
                }
                else
                {
                    std::cout << "Error: receiving data" << std::endl;
                }
            }
        }
    }
}

void Server::startServer()
{
    createSocket();
    initAdress();
    startListen();
    runPoll();
}

Server::~Server() {}
