#pragma once
#include <cstdlib>
#include <string>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "Client.hpp"
#include <csignal>

class Client;

class Server
{
private:
    Server *_server_instance;
    int _listening_socket;
    std::vector<pollfd> _poll_fds;
    std::map<int, Client> _clients;
    void _makeNonBlock(int sock_fd);
public:
    void CloseServer(int signal);
    void startServer();
    void createSocket();
    void initAdress();
    void startListen();
    void runPoll();
    void handleNewConect();
    Server();
    ~Server();
};

