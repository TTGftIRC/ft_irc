#pragma once
#include "Server.hpp"
#include <set>

class Server;
class Client;

//macros for error codes
#define RPL_WELCOME(nick) (std::string(":ircserver 001 Welcome to the server, ") + nick + "!\r\n")
#define ERR_NEEDMOREPARAMS(client, command) (std::string(":ircserver 461 ") + client + " " + command + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(client) (std::string(":ircserver 462 ") + client + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH(client) (std::string(":ircserver 464 ") + client + " :Password incorrect\r\n")
#define ERR_NONICKNAMEGIVEN(client) (std::string(":ircserver 431 ") + client + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(client, nick) (std::string(":ircserver 432 ") + client + " " + nick + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(client, nick) (std::string(":ircserver 433 ") + client + " " + nick + " :Nickname is already in use\r\n")
#define ERR_NOORIGIN(client) (std::string("ircserver 409 ") + client + " :No origin specified\r\n")
//ERR_NICKCOLLISION I am not sure if we have to and how to handle it

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
    PING,
    UNKNOWN
};

cmds getCommandEnum(const std::string& cmd);
parsedCmd parseInput(const std::string& input, Client* client);
void _handleClientMessage(Server& server, Client* client, const std::string& cmd);
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
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class NickCommand : public ICommand {
    private:
        bool validChars(const std::string _nick) const;
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class UserCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class JoinCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class PartCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class PrivmsgCommand : public ICommand {
private:
    void handleChannelMessage(Server& server, Client* sender, const std::string& channelName , const std::string& messgae) const;
    void handlePrivateMessage(Server& server, Client* sender, const std::string& targetNickname , const std::string& message) const;
    std::vector<std::string> parseTargets(const std::string& targetsString) const;
public:
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class QuitCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class KickCommand : public ICommand {
private:
    void kickFromChannel(Server& server, Client* sender, 
                                  const std::string& channelName, 
                                  const std::string& targetNick, 
                                  const std::string& reason) const;
public:
    void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class InviteCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class TopicCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class ModeCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class PingCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

std::vector<std::string> splitByComma(const std::string& arg);

bool isValidChannelName(const std::string& name);

bool isNum(const char* input);