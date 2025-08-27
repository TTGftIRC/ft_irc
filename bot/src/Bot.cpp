#include "../inc/Bot.hpp"

Bot::SuperException::SuperException(const std::string &err) : msg(err) { }

Bot::SuperException::~SuperException() throw() {}

const char* Bot::SuperException::what() const throw() { return msg.c_str(); }

Bot::Bot(int port, std::string pass, bool *sig) : _sig(sig), _port(port), _pass(pass), _socket(-1) {
    if (initBot()) {
        loopBot();
    }
}

Bot::~Bot() {}

void Bot::sendMessage(const std::string& msg) {
    std::string out = msg + "\r\n";
    if (send(_socket, out.c_str(), out.size(), 0) < 0) {
        throw SuperException("send() failed");
    }
}

void Bot::sendMsgToClient(const std::string msg, const std::string nick) {
    sendMessage("PRIVMSG " + nick + " :" + msg);
}

void Bot::loginBot() {
    sendMessage("PASS " + _pass);
    sendMessage("NICK Bot42");
    sendMessage("USER bot42 0 * :");
}

void Bot::makeNonBlock() {
    int flags = fcntl(_socket, F_GETFL, 0);
    if (flags == -1) {
        throw SuperException("fcntl F_GETFL");
        return;
    }
    if (fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw SuperException("fcntl F_SETFL");
    }
}

bool Bot::initBot() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) throw SuperException("Failed to create socket");
    struct hostent* server = gethostbyname("127.0.0.1");
    if (!server) {
        throw SuperException("No such host");
        return false;
    }
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(_port);
    if (connect(_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        throw SuperException("Unable to connect");
        return false;
    }
    makeNonBlock();
    loginBot();
    return true;
}

cmds getCommandEnum(std::string message) {
    if (message == "!help") return HELP;
    if (message == "!hello") return HELLO;
    if (message == "!time") return TIME;
    if (message == "!dice") return DICE;
    if (message == "!coin") return COIN;
    return UNKNOWN;
}

parsedMsg parseLine(const std::string& line) {
    std::istringstream iss(line);
    parsedMsg result; // empty struct
    std::string token;
    while (iss >> token) {
        if (token[0] == '!') {
            result.command = token;
            break;
        }
        result.clientNick = token;
    }
    return result;
}

void Bot::handleMessage(std::string line) {
    size_t pos;
    if ((pos = line.find("PING")) != std::string::npos) {
        sendMessage("PONG :" + line.substr(pos, line.length()));
        return;
    }

    if ((pos = line.find("PRIVMSG")) == std::string::npos) {
        return;
    }
    std::string message;

    message = line.substr(pos + 7, line.length());
    parsedMsg parsed = parseLine(message);
    std::cout << "parsed: " << parsed.command << " " << parsed.clientNick << std::endl;
    cmds CommandEnum = getCommandEnum(parsed.command);
    switch (CommandEnum) {
        case HELP: {
            
            break;
        }
        case HELLO: {
            sendMsgToClient("hello", parsed.clientNick);
            break;
        }
        case TIME: {

            break;
        }
        case DICE: {
            break;
        }
        case COIN: {
            break;
        }
        case UNKNOWN: {
            sendMsgToClient("try !help", parsed.clientNick);
            break;
        }
        default: {
            break;
        }
    }
}

void Bot::loopBot() {
    char buf[MAX_SIZE + 1] = {0};
    static int login = 1;

    struct pollfd pfd;
    pfd.fd = _socket;
    pfd.events = POLLIN;
    pfd.revents = 0;

    while (!*_sig) {
        int ret = poll(&pfd, 1, -1);
        if (ret < 0) {
            if (errno == EINTR) continue;
            throw SuperException("poll() failed");
        }
        if (pfd.revents & POLLIN) {
            ssize_t bytes = recv(_socket, buf, sizeof(buf) - 1, 0);
            if (bytes < 0) {
                throw SuperException("recv() failed");
            }
            if (bytes == 0) {
                std::cerr << "Server closed connection." << std::endl;
                break;
            }

            buf[bytes] = '\0';
            _recv_buffer.append(buf, bytes);
            size_t pos;
            while ((pos = _recv_buffer.find("\r\n")) != std::string::npos) {
                std::string line = _recv_buffer.substr(0, pos);
                _recv_buffer.erase(0, pos + 2);
    
                std::cout << "recieved data: " << line << std::endl;
    
                if (login == 1) {
                    if (line.find(":ircserver 464") != std::string::npos) {
                        throw SuperException("Incorrect password");
                        return;
                    } else if (line.find(":ircserver 001") != std::string::npos) {
                        login = 0;
                    }
                }
                handleMessage(line);
            }
        }
    }
}