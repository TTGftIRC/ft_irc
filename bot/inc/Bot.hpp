#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <csignal>

#define MAX_SIZE 512
extern volatile sig_atomic_t sig_recieved;

class Bot {
    private:
        int _port;
        std::string _pass;
        int _socket;
        std::string _recv_buffer;
        bool initBot();
        void loopBot();
        void loginBot();
        void sendMessage(const std::string& msg);
        void handleMessage(std::string line);
    public:
        class SuperException : public std::exception {
            private:
                std::string msg;
            public:
                SuperException(const std::string &err);
                virtual ~SuperException() throw();
                const char* what() const throw();
        };
        Bot(int port, std::string pass);
        ~Bot();
};

