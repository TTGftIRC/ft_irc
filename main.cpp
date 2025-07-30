#include "inc/Server.hpp"
#include "inc/Command.hpp"

volatile sig_atomic_t sig_received = 0;

bool isNum(const char* input) {
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (!std::isdigit(input[i])) {
            return false;
        }
    }
    return true;
}

void handle_sig(int signal) {
    (void)signal;
    sig_received = 1;
}

int main(int ac, char **av) {
    int port;
    std::string pass;
    std::signal(SIGINT, handle_sig);
    std::signal(SIGTERM, handle_sig);

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
