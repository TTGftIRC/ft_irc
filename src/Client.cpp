#include "../inc/Client.hpp"

Client::Client(int client_fd) : _client_fd(client_fd)
{
    std::cout << "new client connection" << _client_fd << std::endl;
}

Client::~Client()
{

}