#include "inc/Bot.hpp"

bool sig_recieved = false;

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
    sig_recieved = true;
}

int main(int ac, char **av) {
    int port;
    std::string pass;
    std::signal(SIGINT, handle_sig);

    if (ac != 3) {
        std::cerr << "Erorr: invalid amount of arguments: try ./bot PORT PASSWORD" << std::endl;
        return 1;
    }
    pass = av[2];
    port = std::atoi(av[1]);
    if (!isNum(av[1])) {
        std::cerr << "Error: Invalid port" << std::endl;
        return 1;
    }

    try {
        Bot bot(port, pass, &sig_recieved);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
