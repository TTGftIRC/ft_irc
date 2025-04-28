#include "../inc/Server.hpp"

void StartServer()
{
    while (42)
    {
        int serverSocket = socket(AF_INET, SOCK_STREAM, );

        sockaddr_in serverAdress;
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_port = htons(8080);
        serverAdress.sin_addr.s_addr = INADDR_ANY;
 
        bind(serverSocket, (struct sockaddr *)&serverAdress, sizeof(serverAdress));

        poll(serverSocket, 3);
        int clientSocket = accept(serverSocket, NULL, NULL);
    
        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout << "Message from client: " << buffer << std::endl;
    }    
}

int main()
{
    StartServer();
}