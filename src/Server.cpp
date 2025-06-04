#include "../inc/Server.hpp"
#include "../inc/Command.hpp"
#include <cstring>

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

Server::Server() {}

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

void Server::requestPollOut(int client_fd, bool enable) {
    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
        if (it->fd == client_fd) {
            if (enable) { 
                it->events |= POLLOUT;
            } else {
                it->events &= ~POLLOUT;
            }
            break;
        }
    }
}

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

void Server::createSocket()
{
    _listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listening_socket == -1)
    {
        std::cerr << "Socket couldn't be created" << std::endl;
        exit(EXIT_FAILURE);
    }
    int option = 1;
    if (setsockopt(_listening_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
    {
        std::cerr << "Error: error SO_REUSEADDR has failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    _makeNonBlock(_listening_socket);
}

void Server::initAdress()
{
    sockaddr_in serverAdress = {};

    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(_port);
    serverAdress.sin_addr.s_addr = INADDR_ANY; // setup serv adress

    // bind port w server
    if (bind(_listening_socket, reinterpret_cast<sockaddr *>(&serverAdress), sizeof(serverAdress)) == -1)
    {
        std::cerr << "Error: bind has failed: " << strerror(errno) << std::endl;
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
    // replc port 8080 w all ports
    std::cout << "Server is now listening on port " << _port << "...." << std::endl;
}

Client* Server::findSecondClient(int sock_src) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getClientFd() != sock_src && it->second->getClientFd() != _listening_socket) {
            return it->second;
        }
    }
    return NULL;
}

void Server::runPoll() {
    pollfd server_fd;
    server_fd.fd = _listening_socket;
    server_fd.events = POLLIN;
    server_fd.revents = 0;
    _poll_fds.push_back(server_fd); // first elem of the pollfd will be the server which will be waiting for new events
    while (true) {
        if (poll(_poll_fds.data(), _poll_fds.size(), -1) == -1) {
            std::cerr << "Error: poll has failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (_poll_fds[0].revents & POLLIN) {
            if (_poll_fds[0].fd == _listening_socket) {
                // handle new client conexions
                sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                int new_socket = accept(_poll_fds[0].fd, reinterpret_cast<sockaddr *>(&client_addr), &addr_len);
                if (new_socket == -1)
                {
                    std::cerr << "Error: accept has failed" << std::endl;
                    exit(EXIT_FAILURE);
                }
                _makeNonBlock(new_socket);
                std::string client_ip = inet_ntoa(client_addr.sin_addr);
                Client* new_client = new Client(new_socket, client_ip, this);
                _clients.insert(std::make_pair(new_socket, new_client));
                pollfd new_conexion;
                new_conexion.fd = new_socket;
                new_conexion.events = POLLIN;
                //delete pollout because I am sending callback for this event and it can lead to infinite search for pollout
                new_conexion.revents = 0;
                _poll_fds.push_back(new_conexion);
            }
        }
        for (size_t i = 1; i < _poll_fds.size(); i++) {
            Client* curr = _clients[_poll_fds[i].fd];
            if (_poll_fds[i].revents & POLLHUP) {
                std::cout << "Client has been disconnected !" << std::endl;
                close(_poll_fds[i].fd);
                delete _clients[_poll_fds[i].fd];
                _clients.erase(_poll_fds[i].fd);
                _poll_fds.erase(_poll_fds.begin() + i);
                --i;
                continue;
            } if (_poll_fds[i].revents & POLLIN) {
                char buffer[1024] = {0};
                size_t bytes_read = recv(_poll_fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytes_read > 0) {
                    curr->appendRecvData(buffer);
                    std::string cmd;
                    while (!(cmd = curr->extractLineFromRecv()).empty()) {
                        std::cout << "RECV " << curr->getClientFd() << ": " << cmd << std::endl;
                        _handleClientMessage(*this, curr, cmd);
                        // Client* target = findSecondClient(curr->getClientFd());
                        // if (target) {
                        //     target->queueMessage(cmd + "\n");
                        // }
                    }
                } else if (bytes_read == 0) {
                    std::cout << "Client has been disconnected !" << std::endl;
                    close(_poll_fds[i].fd);
                    delete _clients[_poll_fds[i].fd];
                    _clients.erase(_poll_fds[i].fd);
                    _poll_fds.erase(_poll_fds.begin() + i);
                    --i;
                } else {
                    std::cerr << "Error: receiving data" << std::endl;
                }
            } if (_poll_fds[i].revents & POLLOUT) {
                if (curr->hasData()) {
                    const std::string& data_to_send = curr->getSendBuf();
                    ssize_t bytes = send(_poll_fds[i].fd, data_to_send.data(), data_to_send.size(), 0);
                    if (bytes > 0) {
                        curr->helpSenderEvent(bytes);
                    } else if (bytes < 0) {
                        std::cerr << "Error: couldn't send msg" << std::endl;
                    }
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

Server::~Server() {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    _clients.clear();
}


Channel* Server::getChannel(const std::string& name) {
    for (std::set<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if ((*it)->getName() == name) {
            return *it;
        }
    }
    return NULL;
}

Channel* Server::getOrCreateChannel(const std::string& name) {
    for (std::set<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if ((*it)->getName() == name) {
            return *it;
        }
    }
    Channel* newChannel = new Channel(name);
    _channels.insert(newChannel);
    std::cout << GREEN << "Channel " << name << " created succesfully" << RESET << std::endl;
    return newChannel;
}

bool Server::addChannel(const std::string& name) {
    for(std::set<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if ((*it)->getName() == name) {
            std::cout << ORANGE << "Channel already exits" << RESET << std::endl;
            return false;
        }
    }
    Channel* newChannel = new Channel(name);
    _channels.insert(newChannel);
    std::cout << GREEN << "Channel " << name << " created succesfully" << RESET << std::endl;
    return true;
}

const std::string& Server::getPass() {
    return _pass;
}

void Server::removeChannel(const std::string& channelName) {
    for (std::set<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if ((*it)->getName() == channelName) {
            delete *it;
            _channels.erase(it);
            break;
        }
    }
}

std::set<Channel*> Server::getChannels() const { return _channels; }
