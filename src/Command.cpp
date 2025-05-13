#include "../inc/Command.hpp"

ICommand::~ICommand() {}

//just hallow function for interface
// void ICommand::execute(Server& server, const parsedCmd& _parsedCmd) {
//     (void)server;
//     (void) _parsedCmd;
// }

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
            if (token[0] == '#') {
                result.channels.insert(token); // add the string following the # to the channel vector
            }
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
// result.channels = {"#general"}
// result.args = {"#general", "hello there"}
// result.srcClient = pointer to sender


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