#include "../inc/Channel.hpp"


Channel::Channel(const std::string& name) : _name(name) {
    std::cout << PURPLE << "Channel " << this->_name << " has been created!" << RESET << std::endl;
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


// void Channel::setTopic(const std::string& topic, const std::string& setter) {
//     if (_topicLocked && _operators.find(setter) == _operators.end()) {
//         Client* client = Server::getClientByNickname(setter); //TODO from client's side a getter by name
//         if (client) {
//             client->sendMessage()
//         }
//         return;
//     }
//     this->_topic = topic;
//     std::cout << BLUE << "Topic has been set!" << RESET << std::endl;
// }

bool Channel::isTopicLocked() const { return this->_topicLocked; }


// bool Channel::addClient(Client* client, const std::string& key) {}
// void Channel::removeClient(const std::string& nickname) {}
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

//Messaging
// void Channel::broadcast(const std::string& message, const std::string& senderNick) {}

//Helpers
size_t Channel::getClientCount() const { return _clients.size(); }
