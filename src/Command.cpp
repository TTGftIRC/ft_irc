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
            // PartCommand partCommand;
            // partCommand.execute(servet, parsed);
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
            client->queueMessage(":ircserver 421 " + client->getNickname() + " " + parsed.cmd + " :Unknown command\r\n");
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
        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " PRIVMSG: Not enough parameters!\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    std::string targetsString = _parsedCmd.args[0]; // channel or client
    std::string message = _parsedCmd.args[1]; //message
    
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1); //eliminate the ':'
    }
    if (message.empty()) {
        std::string errorMessage = ":ircserver 412 " + sender->getNickname() + " :No text to send!\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //parse multiple targets, separated by commas
    std::vector<std::string> targets = parseTargets(targetsString);
    // check if we have any targets
    if (targets.empty()) {
        std::string errorMessage = ":ircserver 411 " + sender->getNickname() + " :No recipient given (PRIVMSG)\r\n";
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
        std::string errorMessage = ":ircserver 403 " + sender->getNickname() + " " + channelName + " :No such channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //check if sender is part of channel
    if (!channel->hasClient(sender->getNickname())) {
        //IRC 404:ERR_CANNOTSENDTOCHAN
        std::string errorMessage = ":ircserver 404 " + sender->getNickname() + channelName + " :Cannot send to channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //let's try to format the message like a propper irc message 
    // Format: :<sender_nick>!<user>@<host> PRIVMSG <channel> :<message>    
    std::string formattedMessage = ":" + sender->getNickname() + "!" + sender->getUsername() +
                                    "@" + sender->getHostname() + " PRIVMSG " + 
                                    channelName + " :" + message + "\r\n";
    channel->broadcast(formattedMessage, sender->getNickname());//send the message to all the channel members but the sender
}

void PrivmsgCommand::handlePrivateMessage(Server& server, Client* sender,
                                            const std::string& targetNick,
                                            const std::string& message) const {
    Client* target = server.getClientByNick(targetNick);
    //if target doesn't exist
    if (target == NULL) {
        //irc 401: ERR_NOSUCHNICK
        std::string errorMessage = ":ircserver 401 " + sender->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    // Format: :<sender_nick>!<user>@<host> PRIVMSG <target_nick> :<message>
    std::string formattedMessage = ":" + sender->getNickname() + "!" + sender->getUsername() + 
                                    "@" + sender->getHostname() + " PRIVMSG " + 
                                    targetNick + " :" + message + "\r\n";
    target->queueMessage(formattedMessage);
}

//PART

void PartCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    if (_parsedCmd.args.size() < 1) {
        //IRC 461:ERR_NEEDMOREPARAMS
        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " PART :Not enough parameters\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //split channels by comma
    std::string channelsString = _parsedCmd.args[0];
    std::vector<std::string> channels;
    std::string current;
    for (size_t i = 0; i < channelsString.length(); ++i) {
        char c = channelsString[i];
        if (c == ',') {
            if (!current.empty()) {
                channels.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        channels.push_back(current);
    }

    // if there is a message like PART #general :Goodbye!
    // that means we will have two args in the _parsedCmd args vector, and we take args[1] as the "reason"
    std::string reason;
    if (_parsedCmd.args.size() > 1) {
        reason = _parsedCmd.args[1];
        if (!reason.empty() && reason[0] == ':') { // this is more strict(we could also skip the checking of ':' at the begining of the reason)
            reason = reason.substr(1);
        } else {
            reason = sender->getNickname(); // if there is no reason , we just pass the name of the client
        }
    }
    for (size_t i = 0; i < channels.size(); ++i) {
        const std::string& channelName = channels[i];
        Channel* channel = server.getChannel(channelName);
        if (channel == NULL) {
            //IRC 403: ERR_NOSUCHCHANNEL
            std::string errorMessage = ":ircserver 403 " + sender->getNickname() + " " + channelName + " :No such channel\r\n";
            sender->queueMessage(errorMessage);
            continue;
        }
        if (!channel->hasClient(sender->getNickname())) {
            //IRC 442: ERR_NOTONCHANNEL
            std::string errorMessage = ":ircserver 442 " + sender->getNickname() + " " + channelName + " :You're not on that channel\r\n";
            sender->queueMessage(errorMessage);
            continue;
        }
        if (channel->getClientCount() > 0 && channel->getOperatorCount() == 0) {
            Client* newOP = channel->getFirstClient();
                if (newOP && channel->addOperator(newOP->getNickname())) {
                    channel->broadcast(newOP->getNickname() + " has become an opperator\r\n");
                    //!!! will change this message , but for now i care about functionality
                    //!!! maybe need to make the message look like a MODE +o message
                    //std::string modeMsg = ":ircserver MODE " + channelName + " +o " + newOP->getNickname() + "\r\n";
                    // channel->broadcast(modeMsg);
                }
        }
        //if no one remains, delete channel
        if (channel->getClientCount() == 0) {
            server.removeChannel(channelName);
        }
    }

}

//KICK

void KickCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    if (_parsedCmd.args.size() < 2) {
        //IRC 461: ERR_NEEDMOREPARAMS
        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " KICK :Not enough parameters\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //sepparate the channels and the users by ','
    std::string channelsString = _parsedCmd.args[0];
    std::vector<std::string> channels;
    std::string currentChannel;
    for (size_t i = 0; i < channelsString.length(); ++i) {
        char c = channelsString[i];
        if (c == ',') {
            if (!currentChannel.empty()) {
                channels.push_back(currentChannel);
                currentChannel.clear();
            }
        } else {
            currentChannel += c;
        }
    }
    if (!currentChannel.empty()) {
        channels.push_back(currentChannel);
        currentChannel.clear();
    }
    std::string usersString = _parsedCmd.args[1];
    std::vector<std::string> users;
    std::string currentUser;
    for (size_t i = 0; i < usersString.length(); ++i) {
        char u = usersString[i];
        if (u == ',') {
            if (!currentUser.empty()) {
                users.push_back(currentUser);
                currentUser.clear();
            }
        } else {
            currentUser += u;
        }
    }
    if (!currentUser.empty()) {
        users.push_back(currentUser);
        currentUser.clear();
    }

    //check for reason, if none, or wrongly set(without :) then the reason will be the operator's nickname
    std::string reason = (_parsedCmd.args.size() > 2) ? _parsedCmd.args[2] : sender->getNickname();
    if (!reason.empty() && reason[0] == ':') {
        reason = reason.substr(1);
    }

    size_t numChannels = channels.size();
    size_t numUsers = users.size();

    if (numChannels == numUsers) {
        for (size_t i = 0; i < numChannels; ++i) {
            kickFromChannel(server, sender, channels[i], users[i], reason);
        }
    } else if (numChannels == 1) {
        for (size_t i = 0; i < numUsers; ++i) {
            kickFromChannel(server, sender, channels[0], users[i], reason);
        }
    } else if (numUsers == 1) {
        for(size_t i = 0; i < numChannels; ++i) {
            kickFromChannel(server, sender, channels[i], users[0], reason);
        }
    }
}

void KickCommand::kickFromChannel(Server& server, Client* sender, 
                                  const std::string& channelName, 
                                  const std::string& targetNick, 
                                  const std::string& reason) const {
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        //IRC 403:ERR_NOSUCHCHANNEL 
        std::string errorMessage = ":ircserver 403 " + sender->getNickname() + " " + channelName + " :No such channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->hasClient(sender->getNickname())) {
        //IRC 442:ERR_NOTONCHANNEL
        std::string errorMessage = "ircserver 442 " + sender->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->isOperator(sender->getNickname())) {
        //IRC 482:ERR_CHANOPRIVSNEEDED
        std::string errorMessage = ":ircserver 482 " + sender->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->hasClient(targetNick)) {
        //IRC 441:ERR_USERNOTINCHANNEL
        std::string errorMessage = ":ircserver 441 " + sender->getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    //!!! ALSO can't kick yourself out of the channel
    if (sender->getNickname() == targetNick) {
        //IRC 482:ERR_CHANOPRIVSNEEDED but personal
        std::string errorMessage = ":ircserver 482 " + sender->getNickname() + " " + channelName + " :You can't kick yourself, use PART instead\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    // Format: :kicker!user@host KICK <channel> <target> :reason
    std::string kickMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname() + " KICK " +
                            channelName + " " + targetNick + " :" + reason + "\r\n";
    channel->broadcast(kickMsg);
    // now remove the target from channel
    channel->removeClient(targetNick);
}