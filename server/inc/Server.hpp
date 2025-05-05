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

class Server
{
private:
    int _listening_socket;
    std::vector<pollfd> _poll_fds;

public:
    void startServer();
    void createSocket();
    void initAdress();
    void startListen();
    void runPoll();
    void handleNewConect();
    Server();
    ~Server();
};

