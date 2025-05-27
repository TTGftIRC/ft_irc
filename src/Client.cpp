#include "../inc/Client.hpp"
#include "../inc/Channel.hpp"
#include "../inc/Server.hpp"

Client::Client(int client_fd, const std::string& hostname, Server* server) : _serv_ref(server), _client_fd(client_fd), _hostname(hostname), _authorized(false), _nickFlag(false), _userFlag(false) {
    std::cout << "new client connection " << _client_fd << std::endl;
}

int Client::getClientFd(void) const {
    return _client_fd;
}

const std::string& Client::getNickname(void) const {
    return _nickname;
}

const std::string& Client::getUsername(void) const {
    return _username;
}

const std::string& Client::getRealname(void) const {
    return _realname;
}

const std::string& Client::getHostname(void) const {
    return _hostname;
}

const std::string& Client::getSendBuf(void) const {
    return _send_buffer;
}

bool Client::getAuth(void) const {
    return _authorized;
}

bool Client::getNickFlag(void) const {
    return _nickFlag;
}

bool Client::getUserFlag(void) const {
    return _userFlag;
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

void Client::setNickFlag(bool flag) {
    _nickFlag = flag;
}

void Client::setUserFlag(bool flag) {
    _userFlag = flag;
}

void Client::appendRecvData(const std::string& buf) {
    size_t len = buf.length();

    if (len >= 2 && buf[len - 2] == '\r' && buf[len - 1] == '\n') {
        _recv_buffer += buf;
        return;
    } else if (len >= 1 && buf[len - 1] == '\n') {
        std::string tmp = buf;
        tmp[len - 1] = '\r';
        tmp += '\n';
        _recv_buffer += tmp;
        return;
    }
    _recv_buffer += buf + "\r\n";
}

//This function is for poll main loop POLLIN mostly for execution of cmds
//Example:
// if (_poll_fds[0].revents & POLLIN) {
//     std::string cmd;
//     while (!(cmd = cur_client->extractLineFromRecv()).empty()) {
//         //now it contains full message without \r\n
//         //Do the parsing now
//     }
// }

std::string Client::extractLineFromRecv() {
    size_t end = _recv_buffer.find("\r\n");
    if (end != std::string::npos) {
        std::string res = _recv_buffer.substr(0, end);
        _recv_buffer.erase(0, end + 2);
        return res;
    }
    return "";
}

//It's for determing if we have data to send as client
bool Client::hasData() const {
    return !_send_buffer.empty();
}

void Client::queueMessage(const std::string& msg) {
    _send_buffer += msg;

    //This is callback for the server to add the event POLLOUT
    _serv_ref->requestPollOut(_client_fd, true);
}

//This a helper function for send() method in server
//Example:
// if (cur_client->hasData()) {
//     const std::string& data_to_send = cur_client->getSendBuf();
//     ssize_t bytes = send(client_fd, data_to_send.data(), data_to_send.length(), 0)

//     if (bytes > 0) {
//         cur_client->helpSenderEvent(bytes);
//         //calls the callback for ending the POLLOUT
//     }
// }

void Client::helpSenderEvent(size_t len) {
    if (len >= _send_buffer.length()) {
        _send_buffer.clear();
    } else {
        _send_buffer.erase(0, len);
    }

    //if buf empty after proccesing buf, disable POLLOUT
    if (_send_buffer.empty()) {
        _serv_ref->requestPollOut(_client_fd, false);
    }
}

bool Client::checkRegistered(void) {
    if (_authorized && _nickFlag && _userFlag) {
        return true;
    }
    return false;
}

Client::~Client(){}
