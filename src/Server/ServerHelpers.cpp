#include "../../inc/Server.hpp"

void Server::disconnectClient(int client_fd) {
    close(client_fd);

    std::map<int, Client*>::iterator clientIt = _clients.find(client_fd);
    if (clientIt != _clients.end()) {
        delete clientIt->second;
        _clients.erase(clientIt);
    }

    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
        if (it->fd == client_fd) {
            std::cout << "Client has been disconnected !" << std::endl;
            _poll_fds.erase(it);
            break;
        }
    }
}

void Server::_makeNonBlock(int sock_fd)
{
    if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "Erorr: fcntl(F_SETFL, O_NONBLOCK) failed";
    }
}

void Server::setPort(int port) {
    _port = port;
}

void Server::setPass(const std::string& pass) {
    _pass = pass;
}
