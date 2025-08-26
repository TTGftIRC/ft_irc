#pragma once
#include <cstdlib>
#include <string>
#include <cstring>
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
#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include <csignal>
#include <ctime>

extern volatile sig_atomic_t sig_received;//exter for visab across files
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
    int listenPoll(struct pollfd *fds, nfds_t nfds, int timeout);
    void handleNewServConnect();
    void CleanClient(int i);
    bool RecvData(int i, Client *curr);
    bool SendData(int i, Client *curr);
    void HandlePollREvents();
    void CleanAllClients();
    void AddToPollStrct(int new_socket, sockaddr_in client_addr);
    std::set<Channel*> getChannels() const;
    Channel* getChannel(const std::string& name);
    Channel* getOrCreateChannel(const std::string& name);
    void removeChannel(const std::string& channelName);
    void CleanAllChannels();
    void handleNewConnection(const std::string& channelName);
    bool addChannel(const std::string& channel);
    // void _handleClientMessage(Client* client, const std::string& cmd);
    Client* getClientByNick(const std::string& nickname);
    Client* findSecondClient(int sock_src);
    void requestPollOut(int client_fd, bool enable);
    void disconnectClient(int client_fd);
    const std::string& getPass();
    std::vector<Client*> getAllClients() const;
    bool isOpOnAnyChannel(const std::string& nick) const;
    Server();
    ~Server();
};
