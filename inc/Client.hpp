#pragma once
#include "Server.hpp"

class Client
{
    private:
        int _client_fd;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _hostname;
        bool _authorized;
    public:
        //getters
        int getClientFd(void);
        const std::string& getNickname(void);
        const std::string& getUsername(void);
        const std::string& getRealname(void);
        const std::string& getHostname(void);
        bool getAuth(void);

        //setters
        void setNickname(const std::string& nickname);
        void setUsername(const std::string& username);
        void setRealname(const std::string& realname);
        void setAuth(bool authorized);

        Client(int client_fd, const std::string& hostname);
        ~Client();
};
