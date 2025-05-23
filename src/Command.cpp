#include "../inc/Command.hpp"
#include <algorithm>

ICommand::~ICommand() {}

void NickCommand::execute(Server& server, const parsedCmd& _parsedCmd) const {
    _parsedCmd.srcClient->setNickname(_parsedCmd.args[1]);
    //and add more logic
    (void)server;
    //voided out so it's compiling
}


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
                result.args.push_back(token.substr(1) + rest); // reconstruct the full trailing argument ("hello there")
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
// result.args = {"#general", "hello there"}
// result.srcClient = pointer to sender

void _handleClientMessage(Client* client, const std::string& cmd) {
    parsedCmd parsed = parseInput(cmd, client);
    cmds CommnadEnum = getCommandEnum(parsed.cmd);
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
            //handle PRIVMSG
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
    //TODO see what to return for unknown command
}


void PrivmsgCommand::execute(Server & server, const parsedCmd& _parsedCmd) const {
    if (_parsedCmd.args.size() < 2) {
        // _parsedCmd.srcClient->sendMessage("ERROR: PRIVMSG requires a target and a message");
        // return;      UNCOMMENT AFTER I HAVE A SENDMESSAGE FROM CLIENT
    }
    std::string target = _parsedCmd.args[0];
    std::string message = _parsedCmd.args[1];
    std::string senderNick = _parsedCmd.srcClient->getNickname();

    (void) server; // just to be able to compile until i get my hands on a server.getChannel(target)(line 63) method TODO

    // If the target of the prvmsg is a channel
    // if (target[0] == '#') {
        // Channel* channel = server.getChannel(target); I NEED THIS, MAYBE WILL IMPLEMENT LATER OR ONE OF YOU GUYS
    // }
    // if (!channel) {
    //     _parsedCmd.srcClient->sendMessage("ERROR: No such channel");
    //     return; 
    // }
    // if (!channel->hasClient(senderNick)) {
    //     _parsedCmd.srcClient->sendMessage("ERROR: You are not on that channel");
    //     return;
    // }
    // std::string fullMessage = ":" + senderNick + " PRIVMSG " + target + " :" + message;
    // channel->broadcast(fullMessage, senderNick);

    // Else assume the PRIVMSG is for another user
    // else {
        // Client* targetClient = server.getClientByNickname(target);
        // if (!targetClient) {
            // _parsedCmd.srcClient->sendMessage("ERROR: No such nickname");
            // return;
        // }
        // std::string fullMessage = ":" + senderNick + " PRIVMSG " + target + " :" + message;
        // targetClient->sendMessage(fullMessage);
    // }
}

