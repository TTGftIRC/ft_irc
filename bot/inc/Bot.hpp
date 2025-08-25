#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

class Bot {
    private:
        int _port;
        std::string _pass;
        int _socket;
        bool initBot();
        void loopBot();
    public:
        Bot(int port, std::string pass);
        ~Bot();
};

