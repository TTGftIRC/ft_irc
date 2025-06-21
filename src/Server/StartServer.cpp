#include "../../inc/Server.hpp"

Server::Server() {}

void Server::createSocket() {
    _listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listening_socket == -1){
        std::cerr << "Socket couldn't be created" << std::endl;
        exit(EXIT_FAILURE);
    }
    int option = 1;
    if (setsockopt(_listening_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0){
        std::cerr << "Error: error SO_REUSEADDR has failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    _makeNonBlock(_listening_socket);
}

void Server::initAdress(){
    sockaddr_in serverAdress = {};

    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(_port);
    serverAdress.sin_addr.s_addr = INADDR_ANY; // setup serv adress

    // bind port w server
    if (bind(_listening_socket, reinterpret_cast<sockaddr *>(&serverAdress), sizeof(serverAdress)) == -1){
        std::cerr << "Error: bind has failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::startListen(){
    if (listen(_listening_socket, 5) == -1){
        std::cerr << "Error: listen has failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    // replc port 8080 w all ports
    std::cout << "Server is now listening on port " << _port << "...." << std::endl;
}


void Server::startServer(){
    createSocket();
    initAdress();
    startListen();
    runPoll();
}
