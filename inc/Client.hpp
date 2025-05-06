#pragma once
#include "Server.hpp"

class Client
{
    private:
        int _client_fd;
    public:
        Client(int client_fd);
        ~Client();
};
