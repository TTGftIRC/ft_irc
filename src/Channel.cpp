#include "../inc/Channel.hpp"


Channel::Channel(const std::string& name) : _name(name) {
    // std::cout << PURPLE << "Channel " << this->_name << " has been created!" << RESET << std::endl;
}

Channel::~Channel() {
    std::cout << ORANGE << "Channel " << this->_name << " has been deleted!" << RESET << std::endl;
    
    std::vector<Client*>::iterator it;                    // don't know yet if i need this, worst chase scenario it's some useless extra lines of code
    for (it = _clients.begin(); it != _clients.end(); ++it) {
        delete *it;
    }
    _clients.clear();
}


const std::string& Channel::getName() const { return this->_name; }

const std::string& Channel::getTopic() const { return this->_topic; }


void Channel::setTopic(const std::string& topic, const std::string& setter) {
    Client* client = _server->getClientByNick(setter);
    if (!client)   // check if the client is in the server map
        return;
    if (_topicLocked && !isOperator(setter)) {  // check if the topic is locked and if the setter is an operator or not
            client->queueMessage("482 " + setter + " " + " : You're not channel operator");
            return;
        }
    this->_topic = topic;
    std::string topicMsg = ":" + setter + " TOPIC " + _name + " :" + topic;
    broadcast(topicMsg); //without the second param, even the setter will be notified with this message
    
    //optional for server console
    std::cout << GREEN << "Topic for channel " << _name << " changed to: " << topic << RESET << std::endl;


}

bool Channel::isTopicLocked() const { return this->_topicLocked; }


bool Channel::addClient(Client* client, const std::string& password) {
    //check if client already in channel
    if (hasClient(client->getNickname())) {
        client->queueMessage("NOTICE " + client->getNickname() + " :You are already in this channel.");
        return false;
    }
    //check if the channel is invite only and if the client is invited
    if (this->_inviteOnly && !isInvited(client->getNickname())) {
        client->queueMessage("473 " + client->getNickname() + " " + _name + " :Channel is invite-only.");
        return false;
    }
    //check if there is a user limit and if the limit has been reached
    if (_userLimit > 0 && getClientCount() >= _userLimit) {
        client->queueMessage("471 " + client->getNickname() + " " + _name + " :Channel is full.");
        return false;
    }
    //check if channel has password and if the provided password is correct
    if (!this->_password.empty() && password != this->_password) {
        client->queueMessage("476 " + client->getNickname() + " " + _name + " :Incorect password.");
        return false;
    }
    //add the client to the channel
    _clients.push_back(client);
    client->queueMessage("JOIN " + _name + " :Welcome to the channel.");
    //broadcast to the other members that a new client joined
    std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN " + _name;
    broadcast(joinMsg, client->getNickname()); // the broadcast will not reach the client who just joined
    //send topic to new client
    client->queueMessage("332 " + client->getNickname() + " " + _name + " :" + _topic);
    return true;
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
    
    //broadcast parting
    std::string partMsg = ":" + nickname + " PART " + _name;
    broadcast(partMsg, nickname);
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


bool Channel::addOperator(const std::string& nickname) {
    return _operators.insert(nickname).second;
    //std::set.insert returns a std::pair<iterator, bool>, so the .second tells you if the insertion was succesful
}

bool Channel::removeOperator(const std::string& nickname) {
    return _operators.erase(nickname) > 0; // 1 if the operator was succesfully removed, 0 if the nickname wasn't an op in the first place
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


void Channel::setPassword(const std::string& password) { _password = password;}

void Channel::removePassword() { _password.clear(); }

void Channel::setUserLimit(int limit) { _userLimit = limit; }

void Channel::SetInviteOnly(bool on) { _inviteOnly = on; }

void Channel::SetTopicLock(bool on) { _topicLocked = on; }


void Channel::broadcast(const std::string& message, const std::string& senderNick) {
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (*it && (*it)->getNickname() != senderNick) {  // so we dont send to the user that is broadcasting the message (irc behavior)
            (*it)->queueMessage(message);
        }
    }
}

//Helpers
size_t Channel::getClientCount() const { return _clients.size(); }
