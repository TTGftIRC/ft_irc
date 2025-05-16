#include "../inc/Client.hpp"

Client::Client(int client_fd, const std::string& hostname, Server* server) : _serv_ref(server), _client_fd(client_fd), _hostname(hostname), _authorized(false), _ack_msg(false) {
    std::cout << "new client connection " << _client_fd << std::endl;
}

int Client::getClientFd(void) {
    return _client_fd;
}

const std::string& Client::getNickname(void) {
    return _nickname;
}

const std::string& Client::getUsername(void) {
    return _username;
}

const std::string& Client::getRealname(void) {
    return _realname;
}

const std::string& Client::getHostname(void) {
    return _hostname;
}

bool Client::getAuth(void) {
    return _authorized;
}

void Client::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

void Client::setUsername(const std::string& username) {
    _username = username;
}

void Client::setRealname(const std::string& realname) {
    _realname = realname;
}

void Client::setAuth(bool authorized) {
    _authorized = authorized;
}

void Client::AppendToBuffer(const std::string &to_append) {
    _send_buffer += to_append;
}

Client::~Client(){}
