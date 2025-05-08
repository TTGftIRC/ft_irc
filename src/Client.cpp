#include "../inc/Client.hpp"

Client::Client(int client_fd) : _client_fd(client_fd)
{
    std::cout << "new client connection " << _client_fd << std::endl;
}

void Client::AppendToBuffer(const std::string &to_append)
{
    _send_buffer += to_append;
}

Client::~Client(){}