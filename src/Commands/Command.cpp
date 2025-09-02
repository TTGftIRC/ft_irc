#include "../../inc/Command.hpp"
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
// so result from PRIVMSG #general #channel :hello there
//                  is
// result.cmd = "PRIVMSG"
// result.args = {"#general", "#channel", ":hello there"}
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

bool _handleClientMessage(Server& server, Client* client, const std::string& cmd) {
    parsedCmd parsed = parseInput(cmd, client);
    cmds CommandEnum = getCommandEnum(parsed.cmd);
    if (!client->checkRegistered() && 
        (CommandEnum != PASS && 
        CommandEnum != NICK && 
        CommandEnum != USER && 
        CommandEnum != CAP && 
        CommandEnum != QUIT && 
        CommandEnum != PING)) {
            std::string clientName = (client->getNickFlag()) ? client->getNickname() : "*";
            std::string errorMsg = ERR_NOTREGISTERED(clientName);
            client->queueMessage(errorMsg);
            return true;
        }
    if (CommandEnum == PRIVMSG || 
        CommandEnum == JOIN || 
        CommandEnum == PART || 
        CommandEnum == MODE || 
        CommandEnum == WHO || 
        CommandEnum == WHOIS || 
        CommandEnum == NICK || 
        CommandEnum == QUIT || 
        CommandEnum == INVITE) {
            client->setLastActivityTime(std::time(NULL));
        }
    switch (CommandEnum) {
        case PASS: { // PASS blablabli
            PassCommand passCommand;
            passCommand.execute(server, parsed);
            break;
        }
        case NICK:{  // NICK tudor
            NickCommand nickCommand;
            nickCommand.execute(server, parsed);
            break;
        }
        case USER: { // USER
            UserCommand userCommand;
            userCommand.execute(server, parsed);
            break;
        }
        case PRIVMSG: {
            PrivmsgCommand privmsgCommand;
            privmsgCommand.execute(server, parsed);
            break;
        }
        case JOIN: { // JOIN #general,#strict,#channel  blablabli,lalala
            JoinCommand joinCommand;
            joinCommand.execute(server, parsed);
            break;
        }
        case PART: { // PART #general :reason(optional)
            PartCommand partCommand;
            partCommand.execute(server, parsed);
            break;
        }
        case QUIT: { // QUIT :reason(optional)
            QuitCommand quitCommand;
            quitCommand.execute(server, parsed);
            return false;
        }
        case KICK: { // KICK #general,#strict tudor,grisha :just because(optional)
            KickCommand kickCommand;
            kickCommand.execute(server, parsed);
            break;
        }
        case INVITE: { // INVITE grisha #general
            InviteCommand inviteCommand;
            inviteCommand.execute(server, parsed);
            break;
        }
        case TOPIC: { // TOPIC #general -- you get the topic   TOPIC #general :caFts -- you set the topic ( can also be empty)
            TopicCommand topicCommand;
            topicCommand.execute(server, parsed);
            break;
        }
        case MODE: { 
            ModeCommand modeCommand;
            modeCommand.execute(server, parsed);
            break;
        }
        /*
        | Mode | Command to Enable      | Command to Disable      |
|------|-----------------------|------------------------|
| i    | MODE #chan +i         | MODE #chan -i          |
| k    | MODE #chan +k pass    | MODE #chan -k          |
| o    | MODE #chan +o nick    | MODE #chan -o nick     |
| l    | MODE #chan +l 5       | MODE #chan -l          |
| t    | MODE #chan +t         | MODE #chan -t          |
        */
        case PING: {
            PingCommand pingCommand;
            pingCommand.execute(server, parsed);
            break;
        }
        // ctrl + Z to susspend a client process and write "fg" (forground) to restore the client session
        case CAP: {
            CapCommand capCommand;
            capCommand.execute(server, parsed);
            break;
        }
        case WHO: {
            WhoCommand whoCommand;
            whoCommand.execute(server, parsed);
            break;
        }
        case WHOIS: {
            WhoIsCommand whoIsCommand;
            whoIsCommand.execute(server, parsed);
            break;
        }
        case UNKNOWN: {
            break;
        }
        default: {
            //client->queueMessage(":ircserver 421 " + client->getNickname() + " " + parsed.cmd + " :Unknown command\r\n");
            break;
        }
    }
    if (parsed.srcClient->checkRegistered() && !parsed.srcClient->getWelcomeMsg()) {
        parsed.srcClient->setSigOnTime(std::time(NULL));
        parsed.srcClient->setWelcomeMsg(true);
        parsed.srcClient->queueMessage(RPL_WELCOME(parsed.srcClient->getNickname(), parsed.srcClient->getUsername(), parsed.srcClient->getHostname()));
    }
    return true;
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
    if (cmd == "CAP") return CAP;
    if (cmd == "WHO") return WHO;
    if (cmd == "WHOIS") return WHOIS;
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
}

//PRIVMSG
void PrivmsgCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    //check if we have a min of 2 args
    if (_parsedCmd.args.size() < 2) {
        // std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " PRIVMSG: Not enough parameters!\r\n";
        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), "PRIVMSG"));
        return;
    }
    std::string targetsString = _parsedCmd.args[0]; // channel or client
    std::string message = _parsedCmd.args[1]; //message
    
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1); //eliminate the ':'
    }
    if (message.empty()) {
        sender->queueMessage(ERR_NOTEXTTOSEND(sender->getNickname()));
        return;
    }
    //parse multiple targets, separated by commas
    std::vector<std::string> targets = parseTargets(targetsString);
    // check if we have any targets
    if (targets.empty()) {
        sender->queueMessage(ERR_NORECIPIENT(sender->getNickname(), _parsedCmd.cmd));
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
        sender->queueMessage(ERR_NOSUCHCHANNEL(sender->getNickname(), channelName));
        return;
    }
    //check if sender is part of channel
    if (!channel->hasClient(sender->getNickname())) {
        sender->queueMessage(ERR_CANNOTSENDTOCHAN(sender->getNickname(), channelName));
        return;
    }
    // Format: :<sender_nick>!<user>@<host> PRIVMSG <channel> :<message>    
    std::string prefix = ":" + sender->getNickname() + "!" + sender->getUsername() +
                                    "@" + sender->getHostname() + " PRIVMSG " + 
                                    channelName + " :";
    std::vector<std::string> messages = splitMessage(prefix, message);
    for (size_t i = 0; i < messages.size(); i++) {
        channel->broadcast(messages[i], sender->getNickname());//send the message to all the channel members but the sender
    }
}

void PrivmsgCommand::infoDCC(const std::string& message) const {
    if (message.find("DCC SEND") == std::string::npos) {
        return;
    }

    size_t pos = message.find("DCC SEND");

    std::istringstream iss(message.substr(pos + 9)); 

    std::string filename;
    std::string ipStr;
    std::string portStr;
    std::string sizeStr;

    if (iss >> filename >> ipStr >> portStr >> sizeStr) {
        unsigned long ipInt = std::strtoul(ipStr.c_str(), NULL, 10);
        struct in_addr ip_addr;
        ip_addr.s_addr = htonl(ipInt); 

        std::string ip = inet_ntoa(ip_addr);
        int port = std::atoi(portStr.c_str());
        int size = std::atoi(sizeStr.c_str());

        std::cout << "DCC SEND Request:\n";
        std::cout << "- File: " << filename << "\n";
        std::cout << "- IP: " << ip << "\n";
        std::cout << "- Port: " << port << "\n";
        std::cout << "- Size: " << size << " bytes\n";
    } else {
        std::cerr << "error in DCC SEND message\n";
    }
}

std::vector<std::string> PrivmsgCommand::splitMessage(const std::string& prefix, const std::string& message) const {
    const size_t IRC_MAX_SIZE = 512;
    std::vector<std::string> messages;

    size_t message_max_size = IRC_MAX_SIZE - prefix.size() - 2;
    size_t pos = 0;
    while (pos < message.size()) {
        size_t len = std::min(message_max_size, message.size() - pos);
        messages.push_back(prefix + message.substr(pos, len) + "\r\n");
        pos += len;
    }

    return messages;
}

void PrivmsgCommand::handlePrivateMessage(Server& server, Client* sender,
                                            const std::string& targetNick,
                                            const std::string& message) const {
    Client* target = server.getClientByNick(targetNick);
    //if target doesn't exist
    if (target == NULL) {
        sender->queueMessage(ERR_NOSUCHNICK(sender->getNickname(), targetNick));
        return;
    }
    if (message.find("\x01" "DCC SEND") != std::string::npos) {
        infoDCC(message);
    }
    // Format: :<sender_nick>!<user>@<host> PRIVMSG <target_nick> :<message>
    std::string prefix = ":" + sender->getNickname() + "!" + sender->getUsername() + 
                                    "@" + sender->getHostname() + " PRIVMSG " + 
                                    targetNick + " :";
    std::vector<std::string> messages = splitMessage(prefix, message);
    for (size_t i = 0; i < messages.size(); i++) {
        target->queueMessage(messages[i]);
    }
}

//PART

void PartCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    if (_parsedCmd.args.size() < 1) {
        // std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " PART :Not enough parameters\r\n";
        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
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
            sender->queueMessage(ERR_NOSUCHCHANNEL(sender->getNickname(), channelName));
            continue;
        }
        if (!channel->hasClient(sender->getNickname())) {
            sender->queueMessage(ERR_NOTONCHANNEL(sender->getNickname(), channelName));
            continue;
        }
        channel->removeClient(sender->getNickname());
        //broadcast parting
        std::string partMsg = ":" + sender->getNickname() + "!" + sender->getUsername() 
                                + "@" + sender->getHostname() + " PART " + channelName + " :" + reason;
        channel->broadcast(partMsg, sender->getNickname());
        sender->queueMessage(partMsg);
        if (channel->getClientCount() > 0 && channel->getOperatorCount() == 0) {
            Client* newOP = channel->getFirstClient();
                if (newOP) {
                    channel->addOperator(newOP->getNickname());
                    channel->broadcast("\n" + newOP->getNickname() + " has become an opperator\r\n");
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
        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
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
    } else {  // if we have for example 2 channels and 3 users
        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
        return;
    }
}

void KickCommand::kickFromChannel(Server& server, Client* sender, 
                                  const std::string& channelName, 
                                  const std::string& targetNick, 
                                  const std::string& reason) const {
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        sender->queueMessage(ERR_NOSUCHCHANNEL(sender->getNickname(), channelName));
        return;
    }
    if (!channel->hasClient(sender->getNickname())) {
        sender->queueMessage(ERR_NOTONCHANNEL(sender->getNickname(), channelName));
        return;
    }
    if (!channel->isOperator(sender->getNickname())) {
        sender->queueMessage(ERR_CHANOPRIVSNEEDED(sender->getNickname(), channelName));
        return;
    }
    if (!channel->hasClient(targetNick)) {
        sender->queueMessage(ERR_USRNOTINCHANNEL( sender->getNickname(), targetNick, channelName));
        return;
    }
    //!!! ALSO can't kick yourself out of the channel
    if (sender->getNickname() == targetNick) {
        std::string errorMessage = ":ircserver 482 " + sender->getNickname() + " " + channelName + " :You can't kick yourself, use PART instead\r\n"; //exception(can't use macro for this special case)
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
        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
        return;
    }

    std::string channelName = _parsedCmd.args[0];
    Channel* channel = server.getChannel(channelName);
    if (!channel) { 
        sender->queueMessage(ERR_NOSUCHCHANNEL(sender->getNickname(), channelName));
        return;
    }
    if (!channel->hasClient(sender->getNickname())) {
        sender->queueMessage(ERR_NOTONCHANNEL(sender->getNickname(), channelName));
        return;
    }
    if (_parsedCmd.args.size() == 1) { // if the user calls just TOPIC #channel 
        if (!channel->getTopic().empty()) { // if the topic on said channel is not empty
            sender->queueMessage(RPL_TOPIC(sender->getNickname(), channel->getName(), channel->getTopic()));
            return;
        }
        else {
            sender->queueMessage(RPL_NOTOPIC(sender->getNickname(), channel->getName()));
            return;
        }

    }
    std::string newTopic = _parsedCmd.args[1];
    if (!newTopic.empty() && newTopic[0] == ':') {
        newTopic = newTopic.substr(1);
    }
    if (channel->isTopicLocked() && !channel->isOperator(sender->getNickname())) {
        // std::string errorMessage = ":ircserver 482 " + sender->getNickname() +  " " + channel->getName() + " :You're not channel operator\r\n";
        sender->queueMessage(ERR_CHANOPRIVSNEEDED(sender->getNickname(), channel->getName()));
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
        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
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
            sender->queueMessage(ERR_BADCHANMASK(sender->getNickname(), channelName));
            continue;
        }
        Channel* channel = server.getOrCreateChannel(channelName);
        //if sender already in channel, continue to next channel(if any left)
        if (channel->hasClient(sender->getNickname())) {
            continue;
        }
        //invite only, sender not invited
        if (channel->isInviteOnly() && !channel->isInvited(sender->getNickname())) {
            sender->queueMessage(ERR_INVITEONLYCHAN(sender->getNickname(), channelName));
            continue;
        }
        //full
        if (channel->isFull()) {
            sender->queueMessage(ERR_CHANNELISFULL(sender->getNickname(), channelName));
            continue;
        }
        //key/password protected
        if (channel->hasPassword() && !channel->verifyPassword(key)) {
            sender->queueMessage(ERR_BADCHANNELKEY(sender->getNickname(), channelName));
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
            sender->queueMessage(RPL_TOPIC(sender->getNickname(), channelName, channel->getTopic()));
        } else {
            sender->queueMessage(RPL_NOTOPIC(sender->getNickname(), channelName));
        }
        //send list of user's names from channel
        std::string nameList = channel->getNameList();
        sender->queueMessage(RPL_NAMEREPLY(sender->getNickname(), channelName, nameList));
        sender->queueMessage(RPL_ENDOFNAMES(sender->getNickname(), channelName));
    }
}

//INVITE

void InviteCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    Client* sender = _parsedCmd.srcClient;
    if (_parsedCmd.args.size() < 2) {
        //461 ERR_NEEDMOREPARAMS
        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
        return;
    }
    std::string targetNick = _parsedCmd.args[0];
    std::string channelName = _parsedCmd.args[1];
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        sender->queueMessage(ERR_NOSUCHCHANNEL(sender->getNickname(), channelName));
        return;
    }
    if (!channel->hasClient(sender->getNickname())) {
        sender->queueMessage(ERR_NOTONCHANNEL(sender->getNickname(), channelName));
        return;
    }
    if (!channel->isOperator(sender->getNickname())) {
        sender->queueMessage(ERR_CHANOPRIVSNEEDED(sender->getNickname(), channelName));
        return;
    }
    Client* target = server.getClientByNick(targetNick);
    if (!target) {
        sender->queueMessage(ERR_NOSUCHNICK(sender->getNickname(), targetNick));
        return;
    }
    if (channel->hasClient(targetNick)) {
        //443 ERR_USERONCHANNEL
        sender->queueMessage(ERR_USERONCHANNEL(sender->getNickname(), targetNick, channelName));
        return;
    }
    if (channel->isInviteOnly() && !channel->isOperator(sender->getNickname())) {
        sender->queueMessage(ERR_CHANOPRIVSNEEDED(sender->getNickname(), channelName));
        return;
    }
    channel->invite(targetNick);
    sender->queueMessage(RPL_INVITING(sender->getNickname(), targetNick, channelName));
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
    std::string modules = "+";
    if (_parsedCmd.args.size() < 1) {
        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
        return;
    }
    if (_parsedCmd.args.size() == 1) {
        if (_parsedCmd.args[0][0] != '#') {
            Client* target = server.getClientByNick(_parsedCmd.args[0]);
            if (!target) {
                sender->queueMessage(ERR_NOSUCHNICK(sender->getNickname(), _parsedCmd.args[0]));
                return;
            } else {
                if (target->getInvisible()) {
                    modules += "i";
                }
                // if (target is op) {
                //     moduel += "o";
                // }
                sender->queueMessage(RPL_UMODEIS(target->getNickname(), modules));
                return;
            }
        } else {
            std::string modules = "+";
            Channel* target = server.getChannel(_parsedCmd.args[0]);
            if (!target) {
                sender->queueMessage(ERR_NOSUCHCHANNEL(sender->getNickname(), _parsedCmd.args[0]));
                return;
            }
            if (target->isInviteOnly())
                modules += "i";
            if (target->hasPassword())
                modules += "k";
            if (target->isTopicLocked())
                modules += "t";
            if (target->getUserLimit() != 0)
                modules += "l";
            sender->queueMessage(RPL_CHANNELMODEIS(sender->getNickname(), target->getName(), modules));
            return;
        }
    }
    //mode for client
    if (_parsedCmd.args[0][0] != '#') {
        if (_parsedCmd.args[1] == "+i" || _parsedCmd.args[1] == "-i") {
            if (_parsedCmd.srcClient->getNickname() != _parsedCmd.args[0]) {
                std::string errorMsg = ERR_USERDONTMATCH(_parsedCmd.srcClient->getNickname());
                _parsedCmd.srcClient->queueMessage(errorMsg);
                return;
            }
            std::string replyMsg = ":" + _parsedCmd.srcClient->getNickname() + "!" + _parsedCmd.srcClient->getUsername() + "@" + _parsedCmd.srcClient->getHostname() + " MODE " + _parsedCmd.args[0] + " :" + _parsedCmd.args[1] + "\r\n";
            if (_parsedCmd.args[1] == "+i") {
                _parsedCmd.srcClient->setInvisible(true);
                _parsedCmd.srcClient->queueMessage(replyMsg);
                return ;
            } else if (_parsedCmd.args[1] == "-i") {
                _parsedCmd.srcClient->setInvisible(false);
                _parsedCmd.srcClient->queueMessage(replyMsg);
                return ;
            }
        }
    }
    std::string channelName = _parsedCmd.args[0];
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        sender->queueMessage(ERR_NOSUCHCHANNEL(sender->getNickname(), channelName));
        return;
    }
    if (!channel->hasClient(sender->getNickname())) {
        sender->queueMessage(ERR_NOTONCHANNEL(sender->getNickname(), channelName));
        return;
    }
    if (!channel->isOperator(sender->getNickname())) {
        sender->queueMessage(ERR_CHANOPRIVSNEEDED(sender->getNickname(), channelName));
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
                        // std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " MODE :Not enough parameters\r\n";
                        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
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
                    // std::string errorMessage = ":ircserver 461 " + sender->getNickname() + " MODE :Not enough parameters\r\n";
                    sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
                    return; 
                }
                std::string target = _parsedCmd.args[index];
                if (!channel->hasClient(target)) {
                    sender->queueMessage(ERR_USRNOTINCHANNEL( sender->getNickname(), target, channelName));
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
                        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
                        return; 
                    }
                    std::string number = _parsedCmd.args[index];
                    if (!isNum(number.c_str())) {
                        sender->queueMessage(ERR_NEEDMOREPARAMS(sender->getNickname(), _parsedCmd.cmd));
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

void CapCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    (void)server;
    _parsedCmd.srcClient->queueMessage("CAP * LS :\r\n");
}

bool isVisible(Client& srcClient, Client& targetClient, bool isChannel, Server& server) {
    if (&srcClient == &targetClient) {
        return true;
    }

    if (isChannel) {
        return true;
    }

    std::set<Channel*> allChannels = server.getChannels();
    for (std::set<Channel*>::iterator it = allChannels.begin(); it != allChannels.end(); ++it) {
            if ((*it)->hasClient(targetClient.getNickname()) && 
            (*it)->hasClient(srcClient.getNickname())) {
                return true;
            }
        }

    if (!targetClient.getInvisible()) {
        return true;
    }
    
    return false;
}

std::string isClientOperator(Client& client, std::set<Channel*> allChannels) {
    for (std::set<Channel*>::iterator it = allChannels.begin(); it != allChannels.end(); ++it) {
        if ((*it)->isOperator(client.getNickname())) {
            return "@";
        }
    }
    return "";
}

void WhoCommand::showUserInfo(Client& srcClient, Client& targetClient, Server& server, bool isChannel) const {
    if (isVisible(srcClient, targetClient, isChannel, server)) {
        srcClient.queueMessage(
            RPL_WHOREPLY(srcClient.getNickname(),
            getClientAllChannels(targetClient, srcClient, server),
            targetClient.getUsername(),
            targetClient.getHostname(),
            targetClient.getNickname(),
            isClientOperator(targetClient, server.getChannels()),
            targetClient.getRealname()));
    }
}

void WhoCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    if (_parsedCmd.args.size() > 1) {
        _parsedCmd.srcClient->queueMessage(ERR_NEEDMOREPARAMS(_parsedCmd.srcClient->getNickname(), _parsedCmd.cmd));
        return;
    }
    if (_parsedCmd.args.size() == 0) {
        std::vector<Client*> allClients = server.getAllClients();
        for (std::vector<Client*>::iterator it = allClients.begin(); it != allClients.end(); ++it) {
            showUserInfo(*_parsedCmd.srcClient, **it, server, false);
        }
    } else {
        if (_parsedCmd.args[0][0] == '#') {
            Channel* channel = server.getChannel(_parsedCmd.args[0]);
            if (channel != NULL) {
                std::vector<Client*> allClients = channel->getUsers();
                for (std::vector<Client*>::iterator it = allClients.begin(); it != allClients.end(); ++it) {
                    showUserInfo(*_parsedCmd.srcClient, **it, server, true);
                }
            }
        } else {
            Client* client = server.getClientByNick(_parsedCmd.args[0]);
            if (client != NULL) {
                showUserInfo(*_parsedCmd.srcClient, *client, server, false);
            }
        }
    }

    _parsedCmd.srcClient->queueMessage(RPL_ENDOFWHO(_parsedCmd.srcClient->getNickname()));
}

std::string getClientAllChannels(Client& targetClient, Client& srcClient, Server& server) {
    std::string resChannels;
    std::set<Channel*> allChannels = server.getChannels();
    if (!targetClient.getInvisible()) {
        for (std::set<Channel*>::iterator it = allChannels.begin(); it != allChannels.end(); ++it) {
            if ((*it)->hasClient(targetClient.getNickname())) {
                resChannels += (*it)->getName() + " ";
            }
        }
    } else {
        for (std::set<Channel*>::iterator it = allChannels.begin(); it != allChannels.end(); ++it) {
            if ((*it)->hasClient(srcClient.getNickname()) && (*it)->hasClient(targetClient.getNickname())) {
                resChannels += (*it)->getName() + " ";
            }
        }
    }
    if (!resChannels.empty() && resChannels[resChannels.size() - 1] == ' ') {
        resChannels.erase(resChannels.size() - 1);
    }
    return resChannels;
}

void WhoIsCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    if (_parsedCmd.args.size() > 1) {
        _parsedCmd.srcClient->queueMessage(ERR_NEEDMOREPARAMS(_parsedCmd.srcClient->getNickname(), _parsedCmd.cmd));
        return;
    } else if (_parsedCmd.args.size() < 1) {
        _parsedCmd.srcClient->queueMessage(ERR_NONICKNAMEGIVEN(_parsedCmd.srcClient->getNickname()));
        return;
    }

    if (!server.getClientByNick(_parsedCmd.args[0])) {
        _parsedCmd.srcClient->queueMessage(ERR_NOSUCHNICK(_parsedCmd.srcClient->getNickname(), _parsedCmd.args[0]));
    } else {
        Client* targetClient = server.getClientByNick(_parsedCmd.args[0]);
        _parsedCmd.srcClient->queueMessage(RPL_WHOISUSER(_parsedCmd.srcClient->getNickname(), targetClient->getNickname(), targetClient->getUsername(), targetClient->getHostname(), targetClient->getRealname()));
        _parsedCmd.srcClient->queueMessage(RPL_WHOISSERVER(_parsedCmd.srcClient->getNickname(), targetClient->getNickname()));
        std::string channels = getClientAllChannels(*targetClient, *_parsedCmd.srcClient, server);
        if (!channels.empty()) {
            _parsedCmd.srcClient->queueMessage(RPL_WHOISCHANNELS(_parsedCmd.srcClient->getNickname(), targetClient->getNickname(), channels));
        }
        _parsedCmd.srcClient->queueMessage(RPL_WHOISIDLE(_parsedCmd.srcClient->getNickname(), targetClient->getNickname(), targetClient->getIdleTime(), std::time(NULL) - targetClient->getSignOnTime()));
    }

    _parsedCmd.srcClient->queueMessage(RPL_ENDOFWHOIS(_parsedCmd.srcClient->getNickname(), _parsedCmd.args[0]));
}
