#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include <set>

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
    Server* _server;
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

    // Topic control
    void setTopic(const std::string &topic, const std::string &setter);
    bool isTopicLocked() const;

    // Client control
    bool addClient(Client *client, const std::string &password);
    void removeClient(const std::string &nickname);
    bool hasClient(const std::string &nickname) const;

    // Operators control
    bool addOperator(const std::string &nickname);
    bool removeOperator(const std::string &nickname);
    bool isOperator(const std::string &nickname) const;

    // Invite system
    void invite(const std::string &nickname);
    bool isInvited(const std::string &nickname) const;

    // Modes
    void setPassword(const std::string &password);
    void removePassword();
    void setUserLimit(int limit);
    void SetInviteOnly(bool on);
    void SetTopicLock(bool on);

    // Messaging
    void broadcast(const std::string &message, const std::string &senderNick = "");
    // !!! the ="" means the parameter is optional , which works great in this case(see setTopic)
    // the feature is called default parameter and it is available in C++98

    // Helpers
    size_t getClientCount() const;
};
