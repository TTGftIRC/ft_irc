#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include <set>
#include <stack>

#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define PURPLE "\033[35m"
#define ORANGE "\033[38;5;208m"
#define RESET "\033[0m"

class Client;
class Server;

class Channel
{
private:
    // Server* _server;
    std::string _name;
    std::string _topic;
    std::string _password;
    size_t _userLimit;
    bool _inviteOnly;
    bool _topicLocked;

    std::vector<Client *> _clients;
    std::set<std::string> _operators; // the operator's nickname
    std::set<std::string> _invited;   // list of invited nicknames
public:
    Channel(const std::string &name);
    ~Channel();

    // getters
    const std::string &getName() const;
    const std::string &getTopic() const;
    size_t getUserLimit() const;
    std::vector<Client*> getUsers() const;
    
    // Topic control
    void setTopic(const std::string &topic, const std::string &setter);
    bool isTopicLocked() const;
    
    // Client control
    void addClient(Client *client);
    void removeClient(const std::string &nickname);
    bool hasClient(const std::string &nickname) const;
    
    // Operators control
    void addOperator(const std::string &nickname);
    void removeOperator(const std::string &nickname);
    bool isOperator(const std::string &nickname) const;
    
    // Invite system
    void invite(const std::string &nickname);
    bool isInvited(const std::string &nickname) const;
    bool isInviteOnly() const;
    
    // Modes
    void setPassword(const std::string &password);
    void removePassword();
    void setUserLimit(size_t limit);
    void setInviteOnly(bool on);
    void setTopicLock(bool on);

    // Messaging
    void broadcast(const std::string &message, const std::string &senderNick = "");
    // !!! the ="" means the parameter is optional , which works great in this case(see setTopic)
    // the feature is called default parameter and it is available in C++98

    // Helpers
    size_t getClientCount() const;
    size_t getOperatorCount() const;
    Client* getFirstClient() const;
    bool isFull() const;
    bool hasPassword() const;
    bool verifyPassword(const std::string& password) const;
    std::string getNameList() const;
};
