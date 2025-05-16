#pragma once
#include "Server.hpp"

class Server;

class Client {
    private:
        Server* _serv_ref;
        int _client_fd;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _hostname;
        std::string _send_buffer;
        std::string _recv_buffer;
        bool _authorized;
    public:
        //getters
        int getClientFd(void);
        const std::string& getNickname(void);
        const std::string& getUsername(void);
        const std::string& getRealname(void);
        const std::string& getHostname(void);
        const std::string& getSendBuf(void);
        bool getAuth(void);

        //setters
        void setNickname(const std::string& nickname);
        void setUsername(const std::string& username);
        void setRealname(const std::string& realname);
        void setAuth(bool authorized);

        bool _ack_msg;


        Client(int client_fd, const std::string& hostname, Server* server);
        ~Client();
        void AppendToBuffer(const std::string &to_append);
        bool hasData() const;
};
