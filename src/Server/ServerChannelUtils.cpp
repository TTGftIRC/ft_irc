#include "../../inc/Server.hpp"
#include "../../inc/Command.hpp"

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


bool Server::isOpOnAnyChannel(const std::string& nick) const {
    for (std::set<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if ((*it)->isOperator(nick))
            return true;
    }
    return false;
}
