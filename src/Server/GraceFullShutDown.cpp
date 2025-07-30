#include "../../inc/Server.hpp"

void Server::CleanClient(int i) {
    if (i >= static_cast<int>(_poll_fds.size()))
        return;

    int fd = _poll_fds[i].fd;

    if (_clients.count(fd)) {
        Client* client = _clients[fd];
        for (std::set<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
            (*it)->removeClient(client->getNickname());
        }

        delete client;
        _clients.erase(fd);
    }

    close(fd);
    _poll_fds.erase(_poll_fds.begin() + i);
}

void Server::CleanAllClients(){
    for (size_t i = _poll_fds.size() - 1; i > 0;i--)//we start claenin from end since last cleand should be the serv
        CleanClient(i);
    close(_poll_fds[0].fd);
    _clients.clear();
    _poll_fds.clear();
}

const std::string& Server::getPass() {
    return _pass;
}

void Server::CleanAllChannels() {
     for (std::set<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete *it;
    }
    _channels.clear();
}

Server::~Server() {
    CleanAllChannels();
    CleanAllClients();
}
