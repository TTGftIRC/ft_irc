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
#include "Command.hpp"

class Client;
class Channel;

class Server
{
private:
    int _port;
    std::string _pass;
    int _listening_socket;
    std::vector<pollfd> _poll_fds;
    std::map<int, Client*> _clients;
    std::set<Channel*> _channels;
    void _makeNonBlock(int sock_fd);
public:
    void setPort(int port);
    void setPass(const std::string& pass);
    void startServer();
    void createSocket();
    void initAdress();
    void startListen();
    void runPoll();
    void handleNewConect();
    std::set<Channel*> getChannels() const;
    Channel* getChannel(const std::string& name);
    Channel* getOrCreateChannel(const std::string& name);
    void removeChannel(const std::string& channelName);
    bool addChannel(const std::string& channel);
    // void _handleClientMessage(Client* client, const std::string& cmd);
    Client* getClientByNick(const std::string& nickname);
    Client* findSecondClient(int sock_src);
    void requestPollOut(int client_fd, bool enable);
    void disconnectClient(int client_fd);
    const std::string& getPass();
    Server();
    ~Server();
};

