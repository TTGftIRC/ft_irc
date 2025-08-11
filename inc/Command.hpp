#pragma once
#include "Server.hpp"
#include <set>

class Server;
class Client;

template<typename T>
std::string toString(T value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

//macros for error codes
#define RPL_WELCOME(nick, user, host) (std::string(":ircserver 001 ") + nick + " :Welcome to the server, " + nick + "[!" + user + "@" + host + "]\r\n")
#define RPL_TOPIC(client, channel, topic) (std::string(":ircserver 332 ") + client + " " + channel + " :" + topic + "\r\n")
#define RPL_NOTOPIC(client, channel) (std::string(":ircserver 331 ") + client + " " + channel + " :No topic is set\r\n")
#define RPL_INVITING(client, target, channel) (std::string(":ircserver 341 ") + client + " " + target + " " + channel + "\r\n")
#define RPL_NAMEREPLY(client, channel, list) (std::string(":ircserver 353 ") + client + " = " + channel + " :" + list + "\r\n")
#define RPL_ENDOFNAMES(client, channel) (std::string(":ircserver 366 ") + client + " " + channel + " :End of /NAMES list.\r\n")
#define ERR_NEEDMOREPARAMS(client, command) (std::string(":ircserver 461 ") + client + " " + command + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(client) (std::string(":ircserver 462 ") + client + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH(client) (std::string(":ircserver 464 ") + client + " :Password incorrect\r\n")
#define ERR_NONICKNAMEGIVEN(client) (std::string(":ircserver 431 ") + client + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(client, nick) (std::string(":ircserver 432 ") + client + " " + nick + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(client, nick) (std::string(":ircserver 433 ") + client + " " + nick + " :Nickname is already in use\r\n")
#define ERR_NOORIGIN(client) (std::string("ircserver 409 ") + client + " :No origin specified\r\n")
#define ERR_NOTEXTTOSEND(client) (std::string("ircserver 412 ") + client + " :No text to send\r\n")
#define ERR_NOSUCHCHANNEL(client, channel) (std::string(":ircserver 403 ") + client + " " + channel + " :No such channel\r\n")
#define ERR_NOSUCHNICK(client, target) (std::string(":ircserver 401 ") + client + " " + target + " :No such nick\r\n")
#define ERR_NORECIPIENT(client, command) (std::string(":ircserver 411 ") + client + " :No recipient given (" + command + ")\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel) (std::string(":ircserver 404 ") + client + " " + channel + " :Cannot send to channel\r\n")
#define ERR_USRNOTINCHANNEL(client, target, channel) (std::string(":ircserver 441 ") + client + " " + target + " " + channel + " :Theu aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(client, channel) (std::string(":ircserver 442 ") + client + " " + channel + " :You're not on that channel\r\n")
#define ERR_USERONCHANNEL(client, target, channel) (std::string(":ircserver 443 ") + client + " " + target + " " + channel + " :is already on channel\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel) (std::string(":ircserver 482 ") + client + " " + channel + " :You're not channel operator\r\n")
#define ERR_BADCHANMASK(client, channel) (std::string(":ircserver 476 ") + client + " " + channel + " :Bad Channel Mask\r\n")
#define ERR_INVITEONLYCHAN(client, channel) (std::string(":ircserver 473 ") + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_CHANNELISFULL(client, channel) (std::string(":ircserver 471 ") + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_BADCHANNELKEY(client, channel) (std::string(":ircserver 475 ") + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_NOTREGISTERED(client) (std::string("ircserver 451 ") + client + " :You have not registered\r\n");
#define ERR_USERDONTMATCH(client) (std::string("ircserver 502 ") + client + " :Cant change mode for other users\r\n")
#define RPL_WHOISUSER(client, nick, username, host, realname) (std::string(":ircserver 311 ") + client + " " + nick + " " + username + " " + host + " * :" + realname + "\r\n")
#define RPL_WHOISSERVER(client, nick) (std::string(":ircserver 312 ") + client + " " + nick + " ircserver :IRC server\r\n")
#define RPL_WHOISCHANNELS(client, nick, channels) (std::string(":ircserver 319 ") + client + " " + nick + " :" + channels + "\r\n")
#define RPL_WHOISIDLE(client, nick, idleTime, signon)(std::string(":ircserver 317 ") + client + " " + nick + " " + toString(idleTime) + " " + toString(signon) + " :seconds idle, signon time\r\n")
#define RPL_ENDOFWHOIS(client, nick) (std::string(":ircserver 318 ") + client + " " + nick + " :End of /WHOIS list\r\n")
#define RPL_WHOREPLY(client, channel, username, host, nick, c_op, realname) (std::string(":ircserver 352 ") + client + " " + channel + " " + username + " " + host + " ircserver " + nick + " H" + c_op + " :0 " + realname + "\r\n")
#define RPL_ENDOFWHO(client) (std::string(":ircserver 315 ") + client + " :End of WHO list\r\n")
#define RPL_UMODEIS(target, flags) (std::string(":ircserver 221 ") + target + " " + flags + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, flags) (std::string(":ircserver 324 ") + client + " " + channel + " " + flags + "\r\n")

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
    CAP,
    WHO,
    WHOIS,
    UNKNOWN
};

cmds getCommandEnum(const std::string& cmd);
parsedCmd parseInput(const std::string& input, Client* client);
bool _handleClientMessage(Server& server, Client* client, const std::string& cmd);
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
    void infoDCC(const std::string& message) const;
    std::vector<std::string> splitMessage(const std::string& prefix, const std::string& message) const;
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

class CapCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

std::string getClientAllChannels(Client& targetClient, Client& srcClient, Server& server);

class WhoCommand : public ICommand {
    private:
        void showUserInfo(Client& srcClient, Client& targetClient, Server& server, bool isChannel) const;
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

class WhoIsCommand : public ICommand {
    public:
        void execute(Server& server, const parsedCmd& _parsedCmd) const;
};

std::vector<std::string> splitByComma(const std::string& arg);

bool isValidChannelName(const std::string& name);

bool isNum(const char* input);