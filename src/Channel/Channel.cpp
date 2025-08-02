#include "../../inc/Channel.hpp"


Channel::Channel(const std::string& name) : _name(name), _userLimit(0), _inviteOnly(false), _topicLocked(false) {
    // std::cout << PURPLE << "Channel " << this->_name << " has been created!" << RESET << std::endl;
}

Channel::~Channel() {
    std::cout << ORANGE << "Channel " << this->_name << " has been deleted!" << RESET << std::endl;
    _clients.clear();
    _operators.clear();
    _invited.clear();
    //here only the clients continer should be removed not the clients pointers, bcs the channel doesn't own the client
}

const std::string& Channel::getName() const { return this->_name; }

const std::string& Channel::getTopic() const { return this->_topic; }

std::string Channel::getNameList() const {
    std::string list;
    for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        Client* client = *it;
        std::string nick = client->getNickname();
        if (isOperator(nick)) {
            list += "@" + nick + " ";
        } else {
            list += nick + " ";
        }
    }
    if (!list.empty() && list[list.length() - 1] == ' ') { // remove any trailing spaces, if present
        list.erase(list.length() - 1, 1);
    }
    return list;
}

std::vector<Client*> Channel::getUsers() const { return this->_clients; }

void Channel::setTopic(const std::string& topic, const std::string& setter) {
    this->_topic = topic;
    //optional for server console
    std::cout << GREEN << "Topic for channel " << _name << " changed to: " << topic << " by " << setter  << "." << RESET << std::endl;
}

bool Channel::isTopicLocked() const { return this->_topicLocked; }


void Channel::addClient(Client* client) {
    //add the client to the channel
    _clients.push_back(client);
}

void Channel::removeClient(const std::string& nickname) {
    std::vector<Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getNickname() == nickname) {
            _clients.erase(it); // removes the pointer from vector, but not the object itself(client)
            break;
        }
    }
    _operators.erase(nickname);   // for both sets, if nickname is not there 
    _invited.erase(nickname);
    //msg to server
    std::cout << ORANGE << "Client " << nickname << " removed from channel " << _name << RESET << std::endl;
}

bool Channel::hasClient(const std::string& nickname) const {
    std::vector<Client*>::const_iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

void Channel::addOperator(const std::string& nickname) {
    _operators.insert(nickname);
}

void Channel::removeOperator(const std::string& nickname) {
    _operators.erase(nickname);
}

bool Channel::isOperator(const std::string& nickname) const {
    return _operators.find(nickname) != _operators.end();
}

void Channel::invite(const std::string& nickname) {
    _invited.insert(nickname);
}

bool Channel::isInvited(const std::string& nickname) const {
    if (_invited.find(nickname) == _invited.end()) {
        return false;
    }
    return true;
}

bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

void Channel::setPassword(const std::string& password) { _password = password;}

void Channel::removePassword() { _password.clear(); }

void Channel::setUserLimit(size_t limit) { _userLimit = limit; }

void Channel::setInviteOnly(bool on) { _inviteOnly = on; }

void Channel::setTopicLock(bool on) { _topicLocked = on; }


void Channel::broadcast(const std::string& message, const std::string& senderNick) {
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (*it && (*it)->getNickname() != senderNick) {  // so we dont send to the user that is broadcasting the message (irc behavior)
            (*it)->queueMessage(message);
        }
    }
}

//Helpers
size_t Channel::getClientCount() const { return _clients.size(); }

size_t Channel::getOperatorCount() const { return _operators.size(); }

Client* Channel::getFirstClient() const { return _clients.empty() ? NULL : *_clients.begin(); }

bool Channel::isFull() const { 
    if (_userLimit == 0) {
        return false;
    }
    return (_clients.size() >= _userLimit); 
}

bool Channel::hasPassword() const { return !_password.empty(); }

bool Channel::verifyPassword(const std::string& password) const {
    return _password == password;
}

size_t Channel::getUserLimit() const { return _userLimit; }