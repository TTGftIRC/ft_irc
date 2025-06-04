#include "../inc/Command.hpp"
#include <algorithm>

ICommand::~ICommand() {}

parsedCmd parseInput(const std::string& input, Client* client) {
    size_t len = input.length();
    std::string trimmed;
    if (len >= 2 && input[len - 2] == '\r' && input[len - 1] == '\n') {
        trimmed = input.substr(0, len - 2);
    } else {
        trimmed = input;
    }
    std::istringstream iss(trimmed);
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

std::vector<std::string> splitByComma(const std::string& arg) {
    std::vector<std::string> result;
    std::string current;
    for (size_t i = 0; i < arg.length(); ++i) {
        char c = arg[i];
        if (c == ',') {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    result.push_back(current);
    return result;
}

bool isValidChannelName(const std::string& name) {
    char prefix = name[0];
    if (prefix != '#' && prefix != '!' && prefix != '+' && prefix != '@') {
        return false;
    }
    for (size_t i = 0; i < name.length(); ++i) {
        char c = name[i];
        if (c == ',' || c == ' ' || c == ':' || c == '\a') { // channels must not have those elements in the name
            return false;
        }
    }
    if (name.length() > 50 ) { //TBD i don't know exactly a clear length limit
        return false;
    }
    return true;
}

void _handleClientMessage(Server& server, Client* client, const std::string& cmd) {
    parsedCmd parsed = parseInput(cmd, client);
    cmds CommnadEnum = getCommandEnum(parsed.cmd);
    switch (CommnadEnum) {
        case PASS: {
            PassCommand passCommand;
            passCommand.execute(server, parsed);
            break;
        }
        case NICK:{
            NickCommand nickCommand;
            nickCommand.execute(server, parsed);
            break;
        }
        case USER: {
            UserCommand userCommand;
            userCommand.execute(server, parsed);
            break;
        }
        case JOIN: {
            JoinCommand joinCommand;
            joinCommand.execute(server, parsed);
            break;
        }
        case PART: {
            PartCommand partCommand;
            partCommand.execute(server, parsed);
            break;
        }
        case PRIVMSG: {
            PrivmsgCommand privmsgCommand;
            privmsgCommand.execute(server, parsed);
            break;
        }
        case QUIT: {
            QuitCommand quitCommand;
            quitCommand.execute(server, parsed);
            break;
        }
        case KICK: {
            KickCommand kickCommand;
            kickCommand.execute(server, parsed);
            break;
        }
        case INVITE: {
            InviteCommand inviteCommand;
            inviteCommand.execute(server, parsed);
            break;
        }
        case TOPIC: {
            TopicCommand topicCommand;
            topicCommand.execute(server, parsed);
            break;
        }
        case MODE: {
            //handle MODE
            break;
        }
        case PING: {
            PingCommand pingCommand;
            pingCommand.execute(server, parsed);
            break;
        }
        case UNKNOWN: {
            if (parsed.cmd == "CAP") {
                parsed.srcClient->queueMessage("CAP * LS :\r\n");
            }
            break;
        }
        default: {
            //client->queueMessage(":ircserver 421 " + client->getNickname() + " " + parsed.cmd + " :Unknown command\r\n");
            break;
        }
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
    if (cmd == "PING") return PING;
    return UNKNOWN;
}

void PassCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    if (_parsedCmd.args.size() != 1) {
        std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
        std::string errorMsg = ERR_NEEDMOREPARAMS(clientName, _parsedCmd.cmd);
        _parsedCmd.srcClient->queueMessage(errorMsg);
        return;
    } else if (_parsedCmd.srcClient->checkRegistered()) {
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

void UserCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    (void)server;
    std::string clientName = (_parsedCmd.srcClient->getNickFlag()) ? _parsedCmd.srcClient->getNickname() : "*";
    if (_parsedCmd.srcClient->checkRegistered()) {
        _parsedCmd.srcClient->queueMessage(ERR_ALREADYREGISTERED(clientName));
        return;
    }

    if (_parsedCmd.args.size() < 4 || _parsedCmd.args[3][0] != ':') {
        _parsedCmd.srcClient->queueMessage(ERR_NEEDMOREPARAMS(clientName, _parsedCmd.cmd));
        return;
    }

    std::string username = _parsedCmd.args[0];
    if (_parsedCmd.args[1].length() > 30) {
        username = username.substr(0, 30);
    }

    std::string realname;
    for (size_t i = 3; i < _parsedCmd.args.size(); ++i) {
        if (i > 3) {
            realname += " ";
        }
        realname += _parsedCmd.args[i];
    }
    realname = realname.substr(1);

    _parsedCmd.srcClient->setUsername(username);
    _parsedCmd.srcClient->setRealname(realname);
    _parsedCmd.srcClient->setUserFlag(true);

    if (_parsedCmd.srcClient->checkRegistered()) {
        _parsedCmd.srcClient->queueMessage(RPL_WELCOME(_parsedCmd.srcClient->getNickname()));
    }
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
    std::vector<std::string> channels = splitByComma(_parsedCmd.args[0]);

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
        if (channelName.empty()) {
            continue;
        }
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
        channel->removeClient(sender->getNickname());
        //broadcast parting
        std::string partMsg = ":" + sender->getNickname() + "!" + sender->getUsername() 
                                + "@" + sender->getHostname() + " PART " + channelName + " :" + reason;
        channel->broadcast(partMsg, sender->getNickname());
        if (channel->getClientCount() > 0 && channel->getOperatorCount() == 0) {
            Client* newOP = channel->getFirstClient();
                if (newOP) {
                    channel->addOperator(newOP->getNickname());
                    channel->broadcast("\n" + newOP->getNickname() + " has become an opperator\r\n");
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
    std::vector<std::string> channels = splitByComma(_parsedCmd.args[0]);
    std::vector<std::string> users = splitByComma(_parsedCmd.args[1]);

    //check for reason, if none, or wrongly set(without :) then the reason will be the operator's nickname
    std::string reason = (_parsedCmd.args.size() > 2) ? _parsedCmd.args[2] : sender->getNickname();
    if (!reason.empty() && reason[0] == ':') {
        reason = reason.substr(1);
    }

    size_t numChannels = channels.size();
    size_t numUsers = users.size();

    if (numChannels == numUsers) {
        for (size_t i = 0; i < numChannels; ++i) {
            if (channels[i].empty() || users[i].empty()) continue;
            kickFromChannel(server, sender, channels[i], users[i], reason);
        }
    } else if (numChannels == 1) {
        for (size_t i = 0; i < numUsers; ++i) {
            if (users[i].empty()) continue;
            kickFromChannel(server, sender, channels[0], users[i], reason);
        }
    } else if (numUsers == 1) {
        for(size_t i = 0; i < numChannels; ++i) {
            if (channels[i].empty()) continue;
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
    // if (!channel->hasClient(sender->getNickname())) {
    //     //IRC 442:ERR_NOTONCHANNEL
    //     std::string errorMessage = ":ircserver 442 " + sender->getNickname() + " " + channelName + " :You're not on that channel\r\n";
    //     sender->queueMessage(errorMessage);
    //     return;
    // }
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
//TOPIC
void TopicCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    if (_parsedCmd.args.size() < 1) {
        //IRC 461: ERR_NEEDMOREPARAMS
        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " TOPIC :Not enough parameters\r\n";
        sender->queueMessage(errorMessage);
        return;
    }

    std::string channelName = _parsedCmd.args[0];
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        //IRC 403:ERR_NOSUCHCHANNEL 
        std::string errorMessage = ":ircserver 403 " + sender->getNickname() + " " + channelName + " :No such channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->hasClient(sender->getNickname())) {
        //IRC 442
        std::string errorMessage = ":ircserver 442 " + sender->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->isOperator(sender->getNickname())) {
        //IRC 482:ERR_CHANOPRIVSNEEDED
        std::string errorMessage = ":ircserver 482 " + sender->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (_parsedCmd.args.size() == 1) { // if the user calls just TOPIC #channel 
        if (!channel->getTopic().empty()) { // if the topic on said channel is not empty
            std::string message = ":ircserver 332 " + sender->getNickname() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n";
            sender->queueMessage(message);
            return;
        }
        else {
            std::string message = ":ircserver 331 " + sender->getNickname() + " " + channel->getName() + " :No topic is set\r\n";
            sender->queueMessage(message);
            return;
        }

    }
    std::string newTopic = _parsedCmd.args[1];
    if (!newTopic.empty() && newTopic[0] == ':') {
        newTopic = newTopic.substr(1);
    }
    if (channel->isTopicLocked() && !channel->isOperator(sender->getNickname())) {
        //IRC 482 ERR_CHANOPRIVSNEEDED
        std::string errorMessage = ":ircserver 482 " + sender->getNickname() +  " " + channel->getName() + " :You're not channel operator\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    channel->setTopic(newTopic, sender->getNickname());//can also be empty , which just erases the previous topic; for now setTopic sends a confirmation to server
    std::string broadcastMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname() + " TOPIC " + channel->getName() + " :" + newTopic + "\r\n";
    channel->broadcast(broadcastMsg);
}

//JOIN
void JoinCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    
    if (_parsedCmd.args.size() < 1 || _parsedCmd.args[0].empty()) {
        //IRC 461 ERR_NEEDMOREPARAMS
        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " JOIN :Not enough parameters\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    std::vector<std::string> channels = splitByComma(_parsedCmd.args[0]);
    std::vector<std::string> keys; 
    if (_parsedCmd.args.size() > 1) {
        keys = splitByComma(_parsedCmd.args[1]);
    }
    for (size_t i = 0; i < channels.size(); ++i) {
        std::string channelName = channels[i];
        std::string key = (i < keys.size()) ? keys[i] : "";
        //validate channel name;
        if (!isValidChannelName(channelName)) {
            std::string errorMessage = ":ircserver 476 " + sender->getNickname() + " " + channelName + " :Bad Channel Mask\r\n";
            sender->queueMessage(errorMessage);
            continue;
        }
        Channel* channel = server.getOrCreateChannel(channelName);
        //if sender already in channel, continue to next channel(if any left)
        if (channel->hasClient(sender->getNickname())) {
            continue;
        }
        //invite only, sender not invited
        if (channel->isInviteOnly() && !channel->isInvited(sender->getNickname())) {
            //irc 473
            std::string errorMessage = ":ircserver 473 " + sender->getNickname() + " " + channelName + " :Cannot join channel (+i)\r\n";
            sender->queueMessage(errorMessage);
            continue;
        }
        //full
        if (channel->isFull()) {
            std::string errorMessage = ":ircserver 471 " + sender->getNickname() + " " + channelName + " :Cannot join channel (+l)\r\n";
            sender->queueMessage(errorMessage);
            continue;
        }
        //key/password protected
        if (channel->hasPassword() && !channel->verifyPassword(key)) {
            std::string errorMessage = ":ircserver 475 " + sender->getNickname() + " " + channelName + " :Cannot join channel (+k)\r\n";
            sender->queueMessage(errorMessage);
            continue;
        }
        //add the sender
        channel->addClient(sender);
        //if first user, make operator
        if (channel->getClientCount() == 1) {
            channel->addOperator(sender->getNickname());
        }
        //broadcast JOIN
        std::string joinMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname() + " JOIN " + channelName + "\r\n";
        channel->broadcast(joinMsg);
        //send topic
         if (!channel->getTopic().empty()) {
            std::string topicMsg = ":ircserver 332 " + sender->getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
            sender->queueMessage(topicMsg);
        } else {
            std::string notopicMsg = ":ircserver 331 " + sender->getNickname() + " " + channelName + " :No topic is set\r\n";
            sender->queueMessage(notopicMsg);
        }
        //send list of user's names from channel
        std::string nameList = channel->getNameList();
        std::string namesMsg = ":ircserver 353 " + sender->getNickname() + " = " + channelName + " :" + nameList + "\r\n";
        sender->queueMessage(namesMsg);
        std::string endMsg = ":ircserver 366 " + sender->getNickname() + " " + channelName + " :End of /NAMES list.\r\n";
        sender->queueMessage(endMsg);
    }
}

//INVITE

void InviteCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    if (_parsedCmd.args.size() < 2) {
        //461 ERR_NEEDMOREPARAMS
        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " INVITE :Not enough parameters\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    std::string targetNick = _parsedCmd.args[0];
    std::string channelName = _parsedCmd.args[1];
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        //403 ERR_NOSUCHCHANNEL
        std::string errorMessage = ":ircserver 403 " + sender->getNickname() + " " + channelName + " :No such channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->hasClient(sender->getNickname())) {
        //IRC 442
        std::string errorMessage = ":ircserver 442 " + sender->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->isOperator(sender->getNickname())) {
        //IRC 482:ERR_CHANOPRIVSNEEDED
        std::string errorMessage = ":ircserver 482 " + sender->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    Client* target = server.getClientByNick(targetNick);
    if (!target) {
        //401 ERR_NOSUCHNICK
        std::string errorMessage = ":ircserver 401 " + sender->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (channel->hasClient(targetNick)) {
        //443 ERR_USERONCHANNEL
        std::string errorMessage = ":ircserver 443 " + sender->getNickname() + " " + targetNick + " " + channelName + " :is already on channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (channel->isInviteOnly() && !channel->isOperator(sender->getNickname())) {
        //482 ERR_CHANOPRIVSNEEDED
        std::string errorMessage = ":ircserver 482 " + sender->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    channel->invite(targetNick);
    // 341 RPL_INVITING
    std::string reply = ":ircservere 341 " + sender->getNickname() + " " + targetNick + " " + channelName + "\r\n";
    sender->queueMessage(reply);
    // Send invite to target
    std::string inviteMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" 
                                + sender->getHostname() + " INVITE " + targetNick + " :" 
                                + channelName + "\r\n";
    target->queueMessage(inviteMsg);
}

//QUIT
void QuitCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    //NO NUMERIC ERROR MESSAGES FOR QUIT, AS QUIT ALWAYS PROCEEDS
    Client* sender = _parsedCmd.srcClient;
    std::string reason;
    if (_parsedCmd.args.size() > 0) {
        reason = _parsedCmd.args[0];
        if (!reason.empty() && reason[0] == ':') {
            reason = reason.substr(1);
        }
    } else {
        reason = "Client exited";
    }
    std::string quitMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                            + " :QUIT " + reason + "\r\n";
    //find all channels in which the client is a user
    std::set<Channel*> channels = server.getChannels();
    for (std::set<Channel*>::const_iterator ch = channels.begin(); ch != channels.end(); ++ch) {
        Channel* channel = *ch;
        if (channel->hasClient(sender->getNickname())) {
            channel->broadcast(quitMsg, sender->getNickname());
            channel->removeClient(sender->getNickname());
            //same as in part, promote new op if needed
            if (channel->getClientCount() > 0 && channel->getOperatorCount() == 0) {
                Client* newOP = channel->getFirstClient();
                if (newOP) {
                    channel->addOperator(newOP->getNickname());
                    channel->broadcast("\n" + newOP->getNickname() + " has become an opperator\r\n");
                }
            }
            //same as in part, if the client leaves behind an empty channel, we delete the channel
            if (channel->getClientCount() == 0) {
                server.removeChannel(channel->getName());
            }
        }
    }
    server.disconnectClient(_parsedCmd.srcClient->getClientFd());
}

//PING

void PingCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    (void)server;
    if (_parsedCmd.args.empty()) {
        _parsedCmd.srcClient->queueMessage(ERR_NOORIGIN(_parsedCmd.srcClient->getNickname()));
        return;
    } // else if (_parsedCmd.args[0][0] != ':') {
    //     _parsedCmd.srcClient->queueMessage(ERR_NEEDMOREPARAMS(_parsedCmd.srcClient->getNickname(), _parsedCmd.cmd));
    //     return;
    // }

    std::string token = _parsedCmd.args[0];
    // token = token.substr(1);
    _parsedCmd.srcClient->queueMessage("PONG :" + token + "\r\n");
}

//MODE

void ModeCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    if (_parsedCmd.args.size() < 2) {
        //461 ERR_NEEDMOREPARAMS
        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " MODE :Not enough parameters\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    std::string channelName = _parsedCmd.args[0];
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        //403 ERR_NOSUCHCHANNEL
        std::string errorMessage = ":ircserver 403 " + sender->getNickname() + " " + channelName + " :No such channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->hasClient(sender->getNickname())) {
        //IRC 442
        std::string errorMessage = ":ircserver 442 " + sender->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    if (!channel->isOperator(sender->getNickname())) {
        //IRC 482:ERR_CHANOPRIVSNEEDED
        std::string errorMessage = ":ircserver 482 " + sender->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        sender->queueMessage(errorMessage);
        return;
    }
    std::string flags = _parsedCmd.args[1];
    char direction = '\0';
    char mode;
    size_t index = 2;
    for (size_t i = 0; i < flags.length(); ++i) {
        if (flags[i] == '+' || flags[i] == '-') {
            direction = flags[i];
            continue;
        }
        mode = flags[i];
        switch (mode) {
            case 'i': {
                if (direction == '+') {
                    channel->setInviteOnly(true);
                    std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + "\r\n";
                    sender->queueMessage(replySenderMsg);
                    std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                            + " MODE " + channelName + " " + direction + mode + "\r\n";
                    channel->broadcast(broadMsg, sender->getNickname());
                } else {
                    channel->setInviteOnly(false);
                    std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + "\r\n";
                    sender->queueMessage(replySenderMsg);
                    std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                            + " MODE " + channelName + " " + direction + mode + "\r\n";
                    channel->broadcast(broadMsg, sender->getNickname());
                }
                break;
            }
            case 't': {
                if (direction == '+') {
                    channel->setTopicLock(true);
                    std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + "\r\n";
                    sender->queueMessage(replySenderMsg);
                    std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                            + " MODE " + channelName + " " + direction + mode + "\r\n";
                    channel->broadcast(broadMsg, sender->getNickname());
                    
                } else {
                    channel->setTopicLock(false);
                    std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + "\r\n";
                    sender->queueMessage(replySenderMsg);
                    std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                            + " MODE " + channelName + " " + direction + mode + "\r\n";
                    channel->broadcast(broadMsg, sender->getNickname());
                }
                break;
            }
            case 'k': {
                if (direction == '-') {
                    channel->removePassword();
                    std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + "\r\n";
                    sender->queueMessage(replySenderMsg);
                    std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                            + " MODE " + channelName + " " + direction + mode + "\r\n";
                    channel->broadcast(broadMsg, sender->getNickname());;
                } else {
                    if (index > _parsedCmd.args.size()) {
                        //461 ERR_NEEDMOREPARAMS
                        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " MODE :Not enough parameters\r\n";
                        sender->queueMessage(errorMessage);
                        return;
                    }
                    std::string argument = _parsedCmd.args[index];
                    channel->setPassword(argument);
                    std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + " " + _parsedCmd.args[index] + "\r\n";
                    sender->queueMessage(replySenderMsg);
                    std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                            + " MODE " + channelName + " " + direction + mode + " " + _parsedCmd.args[index] + "\r\n";
                    channel->broadcast(broadMsg, sender->getNickname());
                    index++;
                }
                break;
            }
            case 'o': {
                if (index > _parsedCmd.args.size()) {
                    //461 ERR_NEEDMOREPARAMS
                    std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " MODE :Not enough parameters\r\n";
                    sender->queueMessage(errorMessage);
                    return; 
                }
                std::string target = _parsedCmd.args[index];
                if (!channel->hasClient(target)) {
                    //ERR_USERNOTINCHANNEL 441
                    std::string errorMessage = ":ircserver 441 " + sender->getNickname() + " " + target + " "
                                                + channelName + " :They aren't on that channel\r\n";
                    sender->queueMessage(errorMessage);
                    return;
                }
                if (direction == '+') {
                    if (!channel->isOperator(target)) {
                        channel->addOperator(target);
                    }
                } else {
                    if (channel->isOperator(target)) {
                        channel->removeOperator(target);
                    }
                }
                std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + " " + _parsedCmd.args[index] + "\r\n";
                sender->queueMessage(replySenderMsg);
                std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                       + " MODE " + channelName + " " + direction + mode + " " + _parsedCmd.args[index] + "\r\n";
                channel->broadcast(broadMsg, sender->getNickname());
                index++;
                break;
            }
            case 'l': {
                if (direction == '-') {
                    if (channel->getUserLimit() != 0) {
                        channel->setUserLimit(0);
                        std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + "\r\n";
                        sender->queueMessage(replySenderMsg);
                        std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                                 + " MODE " + channelName + " " + direction + mode + "\r\n";
                        channel->broadcast(broadMsg, sender->getNickname());;
                    }
                } else {
                    if (index > _parsedCmd.args.size()) {
                        //461 ERR_NEEDMOREPARAMS
                        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " MODE :Not enough parameters\r\n";
                        sender->queueMessage(errorMessage);
                        return; 
                    }
                    std::string number = _parsedCmd.args[index];
                    if (!isNum(number.c_str())) {
                        std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " MODE :Not enough parameters\r\n";
                        sender->queueMessage(errorMessage);
                    }
                    std::stringstream ss(number);
                    size_t limit;
                    ss >> limit;
                    channel->setUserLimit(limit);
                    std::string replySenderMsg = ":ircserver MODE " + channelName + " " + direction + mode + " " + _parsedCmd.args[index] + "\r\n";
                    sender->queueMessage(replySenderMsg);
                    std::string broadMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname()
                                               + " MODE " + channelName + " " + direction + mode + " " + _parsedCmd.args[index] + "\r\n";
                    channel->broadcast(broadMsg, sender->getNickname());
                    index++;
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}