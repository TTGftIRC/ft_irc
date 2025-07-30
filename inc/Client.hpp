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
        bool _invisible;
        bool _welcomeMsg;
        std::time_t _signOnTime;
        std::time_t _lastActivityTime;
    public:
        //getters
        int getClientFd(void) const;
        const std::string& getNickname(void) const;
        const std::string& getUsername(void) const;
        const std::string& getRealname(void) const;
        const std::string& getHostname(void) const;
        std::string& getSendBuf(void) ;
        bool getAuth(void) const;
        bool getNickFlag(void) const;
        bool getUserFlag(void) const;
        bool getInvisible(void) const;
        bool getWelcomeMsg(void) const;
        std::time_t getSignOnTime(void) const;
        std::time_t getIdleTime(void) const;

        //setters
        void setNickname(const std::string& nickname);
        void setUsername(const std::string& username);
        void setRealname(const std::string& realname);
        void setAuth(bool authorized);
        void setNickFlag(bool flag);
        void setUserFlag(bool flag);
        void setInvisible(bool flag);
        void setWelcomeMsg(bool flag);
        void setSigOnTime(std::time_t signOnTime);
        void setLastActivityTime(std::time_t lastActivityTime);

        Client(int client_fd, const std::string& hostname, Server* server);
        ~Client();
        //recv functions
        void appendRecvData(const char *buf, size_t len);
        std::string extractLineFromRecv();
        //send functions
        bool hasData() const;
        void queueMessage(const std::string& msg);
        void helpSenderEvent(size_t len);
        bool checkRegistered(void);
};
