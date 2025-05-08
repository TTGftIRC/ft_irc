#pragma once
#include "Server.hpp"

class Client
{
    private:
        int _client_fd;
        std::string _send_buffer;
    public:
        Client(int client_fd);
        ~Client();
        void AppendToBuffer(const std::string &to_append);
};
