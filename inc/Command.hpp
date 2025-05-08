#pragma once
#include "Server.hpp"

class Server;
class Client;

// stuff like in minishell kinda
// we have parsed command with name
// then arguments
// then client who is calling the command

struct parsedCmd {
    std::string cmd;
    std::vector<std::string> args;
    Client* srcClient;
};

//maybe we can do a switch case later on using enums

enum cmds {
    PASS,
    NICK,
    USER,
    JOIN,
    PART,
    PRIVMSG,
    QUIT,
    KICK,
    INVITE,
    TOPIC,
    MODE
};

class ICommand {
    public:
        virtual ~ICommand();
        virtual void execute(Server& server, const parsedCmd& _parsedCmd);
};

//next encapsulated command which we are going to do with polymorphism (just more classes)
//and they are going to be without constructor so we can just call them
//also in the parsing we can just maybe make switch case using enum

//I will do just a example command (incorrect behaviour anyways because we don't have parser yet)

class NickCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd);
};
