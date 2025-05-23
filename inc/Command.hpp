#pragma once
#include "Server.hpp"
#include <set>

class Server;
class Client;

struct parsedCmd {
    std::string cmd;  //command itself
    std::vector<std::string> args;  // all arguments, including channel names and trailing messages
    Client* srcClient; // who sent the command
};

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
    MODE,
    UNKNOWN
};

cmds getCommandEnum(const std::string& cmd);
parsedCmd parseInput(const std::string& input, Client* client);
void _handleClientMessage(Client* client, const std::string& cmd);
class ICommand {
    public:
        virtual ~ICommand();
        virtual void execute(Server& server, const parsedCmd& _parsedCmd) const = 0;
};

//next encapsulated command which we are going to do with polymorphism (just more classes)
//and they are going to be without constructor so we can just call them
//also in the parsing we can just maybe make switch case using enum

//I will do just a example command (incorrect behaviour anyways because we don't have parser yet)

class PassCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class NickCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class UserCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class JoinCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class PartCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class PrivmsgCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class QuitCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class KickCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class InviteCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class TopicCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class ModeCommand : public ICommand {
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};
