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
        bool _nickFlag;
        bool _userFlag;
    public:
        //getters
        int getClientFd(void) const;
        const std::string& getNickname(void) const;
        const std::string& getUsername(void) const;
        const std::string& getRealname(void) const;
        const std::string& getHostname(void) const;
        const std::string& getSendBuf(void) const;
        bool getAuth(void) const;
        bool getNickFlag(void) const;
        bool getUserFlag(void) const;

        //setters
        void setNickname(const std::string& nickname);
        void setUsername(const std::string& username);
        void setRealname(const std::string& realname);
        void setAuth(bool authorized);
        void setNickFlag(bool flag);
        void setUserFlag(bool flag);

        Client(int client_fd, const std::string& hostname, Server* server);
        ~Client();
        //recv functions
        void appendRecvData(const std::string& buf);
        std::string extractLineFromRecv();
        //send functions
        bool hasData() const;
        void queueMessage(const std::string& msg);
        void helpSenderEvent(size_t len);
        bool checkRegistered(void);
};
