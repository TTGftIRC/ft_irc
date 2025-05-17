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
#include <set>
#include <cstring>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server
{
private:
    int _listening_socket;
    std::vector<pollfd> _poll_fds;
    std::map<int, Client> _clients;
    std::set<Channel*> _channels;
    void _makeNonBlock(int sock_fd);
public:
    void startServer();
    void createSocket();
    void initAdress();
    void startListen();
    void runPoll();
    void handleNewConect();
    Client* getClientByNickname(const std::string& nickname);
    Channel* getChannel(const std::string& name);
    bool addChannel(const std::string& channel);

    Server();
    ~Server();
};

