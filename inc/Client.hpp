#pragma once
#include "Server.hpp"

class Client {
    private:
        int _client_fd;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _hostname;
        std::string _send_buffer;
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

        bool _ack_msg;

        void sendMessage(const std::string& message, int sock_target) const;

        Client(int client_fd, const std::string& hostname);
        Client() : _client_fd(-1) {}
        ~Client();
        void AppendToBuffer(const std::string &to_append);
};
