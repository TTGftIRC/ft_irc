#include "../inc/Command.hpp"
#include <algorithm>

ICommand::~ICommand() {}

parsedCmd parseInput(const std::string& input, Client* client) {
    std::istringstream iss(input);
    parsedCmd result; //empty struct
    result.srcClient = client; // assigned the source client so the command knows who sent it

    std::string token; 
    if (iss >> result.cmd) {
        while (iss >> token) {
            if (token[0] == ':') {  // example: PRIVMSG #general :hello there  
                std::string rest;
                std::getline(iss, rest);
                result.args.push_back(token + rest); // reconstruct the full trailing argument (":hello there")
                break;
            }
            result.args.push_back(token);  // if the token doesn't start with : just push as regular arg
        }
    }
    return result; //return the now filled struct
}
// so result from PRIVMSG #general :hello there
//                  is
// result.cmd = "PRIVMSG"
// result.args = {"#general", ":hello there"}
// result.srcClient = pointer to sender

void _handleClientMessage(Server& server, Client* client, const std::string& cmd) {
    parsedCmd parsed = parseInput(cmd, client);
    cmds CommnadEnum = getCommandEnum(parsed.cmd);
    (void)server;
    switch (CommnadEnum) {
        case PASS:
            //handle PASS
            break;
        case NICK:
            //handle NICK
            break;
        case USER:
            //handle USER
            break;
        case JOIN:
            //handle JOIN
            break;
        case PART:
            //handle PART
            break;
        case PRIVMSG:
            // PrivmsgCommand privmsgCommand;
            // privmsgCommand.execute(server, parsed);
            break;
        case QUIT:
            //handle QUIT
            break;
        case KICK:
            //handle KICK
            break;
        case INVITE:
            //handle INVITE
            break;
        case TOPIC:
            //handle TOPIC
            break;
        case MODE:
            //handle MODE
            break;
        case UNKNOWN:
        default:    
            client->queueMessage("421 " + client->getNickname() + " " + parsed.cmd + " :Unknown command\r\n");
            break;
    }
}

cmds getCommandEnum(const std::string& cmd) {
    if (cmd == "PASS") return PASS;
    if (cmd == "NICK") return NICK;
    if (cmd == "USER") return USER;
    if (cmd == "JOIN") return JOIN;
    if (cmd == "PART") return PART;
    if (cmd == "PRIVMSG") return PRIVMSG;
    if (cmd == "QUIT") return QUIT;
    if (cmd == "KICK") return KICK;
    if (cmd == "INVITE") return INVITE;
    if (cmd == "TOPIC") return TOPIC;
    if (cmd == "MODE") return MODE;
    return UNKNOWN;
}

void PassCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    if (_parsedCmd.args.size() != 1) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string errorMsg = ERR_NEEDMOREPARAMS(clientName, _parsedCmd.cmd);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    } else if (_parsedCmd.srcClient->getAuth()) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string errorMsg = ERR_ALREADYREGISTERED(clientName);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    } else if (_parsedCmd.args[0] != server.getPass()) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string errorMsg = ERR_PASSWDMISMATCH(clientName);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    }
    _parsedCmd.srcClient->setAuth(true);
}

bool NickCommand::validChars(const std::string _nick) const {
    if (!isalpha(_nick[0])) {
        return false;
    }

    for (std::string::const_iterator it = _nick.begin(); it != _nick.end(); ++it) {
        char c = *it;

        if (!isalnum(c) &&
            c != '-' && c != '_' &&
            c != '[' && c != ']' &&
            c != '\\' && c != '`' &&
            c != '^' && c != '{' && c != '}') {
            return false;
        }
    }
    return true;
}

void NickCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    if (_parsedCmd.args.size() < 1) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string errorMsg = ERR_NONICKNAMEGIVEN(clientName);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    } else if (_parsedCmd.args.size() > 1) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string cmd;
        for (std::vector<std::string>::const_iterator it = _parsedCmd.args.begin(); it != _parsedCmd.args.end(); ++it) {
            cmd += *it;
        }
        std::string errorMsg = ERR_ERRONEUSNICKNAME(clientName, cmd);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    } else if (_parsedCmd.args[0].length() > 30) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string errorMsg = ERR_ERRONEUSNICKNAME(clientName, _parsedCmd.args[0]);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    } else if (!validChars(_parsedCmd.args[0])) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string errorMsg = ERR_ERRONEUSNICKNAME(clientName, _parsedCmd.args[0]);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    } else if (server.getClientByNick(_parsedCmd.args[0])) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string errorMsg = ERR_NICKNAMEINUSE(clientName, _parsedCmd.args[0]);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    }
    _parsedCmd.srcClient->setNickname(_parsedCmd.args[0]);
    _parsedCmd.srcClient->setNickFlag(true);
}

//PRIVMSG
void PrivmsgCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    //check if we have a min of 2 args
    if (_parsedCmd.args.size() < 2) {
        std::string errorMessage = "461 " + sender->getNickname() + " PRIVMSG: Not enough parameters!\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    std::string targetsString = _parsedCmd.args[0]; // channel or client
    std::string message = _parsedCmd.args[1]; //message
    
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1); //eliminate the ':'
    }
    if (message.empty()) {
        std::string errorMessage = "412 " + sender->getNickname() + " :No text to send!\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //parse multiple targets, separated by commas
    std::vector<std::string> targets = parseTargets(targetsString);
    // check if we have any targets
    if (targets.empty()) {
        std::string errorMessage = "411 " + sender->getNickname() + " :No recipient given (PRIVMSG)\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    for (std::vector<std::string>::const_iterator it = targets.begin(); it != targets.end(); ++it) {
        const std::string target = *it;
        if (target.empty()) {
            continue;   // if one target is empty just skip it
        }
        //now we decide if target is a channel or a client/user
        if (target[0] == '#' || target[0] == '+' || target[0] == '!' || target[0] == '&') {
            // target == channel
            handleChannelMessage(server, sender, target, message);
        }
        else {
            //target == user
            handlePrivateMessage(server, sender, target, message);
        }
    }
}

std::vector<std::string> PrivmsgCommand::parseTargets(const std::string& targetsString) const {
    std::vector<std::string> targets;
    std::string current;

    //we separate targets by commas
    for (size_t i = 0; i < targetsString.length(); ++i) {
        char c = targetsString[i];

        if (c == ',') {
            //add the current target to the vector if not empty
            if (!current.empty()) {
                targets.push_back(current);
                current.clear();
            }
        } else {
            //add the char to the current target;
            current += c;
        }
    }
    //add last target to the vector, if not empty
    if (!current.empty()) {
        targets.push_back(current);
    }
    return targets;
}

void PrivmsgCommand::handleChannelMessage(Server& server, Client* sender,
                                            const std::string& channelName, 
                                                const std::string& message) const {
    Channel* channel = server.getChannel(channelName);
    if (channel == NULL) {
        //IRC 403:ERR_NOSUCHCHANNEL
        std::string errorMessage = "403 " + sender->getNickname() + " " + channelName + " :No such channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //check if sender is part of channel
    if (!channel->hasClient(sender->getNickname())) {
        //IRC 404:ERR_CANNOTSENDTOCHAN
        std::string errorMessage = "404 " + sender->getNickname() + channelName + " :Cannot send to channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //let's try to format the message like a propper irc message 
    // Format: :<sender_nick>!<user>@<host> PRIVMSG <channel> :<message>    
    std::string formattedMessage = ":" + sender->getNickname() + "!" + sender->getUsername() +
                                    "@" + sender->getHostname() + " PRIVMSG" + 
                                    channelName + " : " + message + "\r\n";
    channel->broadcast(formattedMessage, sender->getNickname());//send the message to all the channel members but the sender
}

void PrivmsgCommand::handlePrivateMessage(Server& server, Client* sender,
                                            const std::string& targetNick,
                                            const std::string& message) const {
    Client* target = server.getClientByNick(targetNick);
    //if target doesn't exist
    if (target == NULL) {
        //irc 401: ERR_NOSUCHNICK
        std::string errorMessage = "401 " + sender->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    // Format: :<sender_nick>!<user>@<host> PRIVMSG <target_nick> :<message>
    std::string formattedMessage = ":" + sender->getNickname() + "!" + sender->getUsername() + 
                                    "@" + sender->getHostname() + " PRIVMSG " + 
                                    targetNick + " :" + message + "\r\n";
    target->queueMessage(formattedMessage);
}
                                