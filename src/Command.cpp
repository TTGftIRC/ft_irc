#include "../inc/Command.hpp"

ICommand::~ICommand() {}

//just hallow function for interface
void ICommand::execute(Server& server, const parsedCmd& _parsedCmd) {
    (void)server;
    (void) _parsedCmd;
}

void NickCommand::execute(Server& server, const parsedCmd& _parsedCmd) {
    _parsedCmd.srcClient->setNickname(_parsedCmd.args[1]);
    //and add more logic
    (void)server;
    //voided out so it's compiling
}
