#include "../../inc/Server.hpp"

Client* Server::getClientByNick(const std::string& nickname) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == nickname) {
            return it->second;
        }
    }
    return NULL;
}

//This is callback for the client side to activate event for POLLOUT
// You can activate and deactivate event

Client* Server::findSecondClient(int sock_src) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getClientFd() != sock_src && it->second->getClientFd() != _listening_socket) {
            return it->second;
        }
    }
    return NULL;
}

std::vector<Client*> Server::getAllClients() const {
    std::vector<Client*> clients;
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        clients.push_back(it->second);
    }
    return clients;
}