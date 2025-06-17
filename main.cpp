#include "inc/Server.hpp"
#include "inc/Command.hpp"

bool isNum(const char* input) {
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (!std::isdigit(input[i])) {
            return false;
        }
    }
    return true;
}

int main(int ac, char **av) {
    int port;
    std::string pass;

    if (ac != 3) {
        std::cerr << "Error: invalid amount of arguments: try ./irc PORT PASSWORD" << std::endl;
        return 1;
    }
    pass = av[2];
    port = std::atoi(av[1]);
    if (!isNum(av[1])) {
        std::cerr << "Error: Invalid port" << std::endl;
        return 1;
    }

    Server server;
    server.setPass(pass);
    server.setPort(port);
    server.startServer();
}
