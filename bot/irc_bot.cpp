#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFER_SIZE 512

void sendLine(int sock, const std::string& msg) {
    std::string out = msg + "\r\n";
    send(sock, out.c_str(), out.size(), 0);
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    const char* server_ip = "127.0.0.1";
    int port = 8080;
    std::string nick = "Bot42";
    std::string user = "botuser";
    std::string pass = "pass";
    std::string channel = "#general";

    // 1. Socket setup
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent* server = gethostbyname(server_ip);
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // 2. Register and join
    sendLine(sock, "PASS " + pass);
    sendLine(sock, "NICK " + nick);
    sendLine(sock, "USER " + user + " 0 * :bot");
    sendLine(sock, "JOIN " + channel);

    char buffer[BUFFER_SIZE + 1];
    std::string leftover;
    while (true) {
        int n = recv(sock, buffer, BUFFER_SIZE, 0);
        if (n <= 0) break;
        buffer[n] = 0;
        leftover += buffer;

        // Parse lines
        size_t pos;
        while ((pos = leftover.find("\r\n")) != std::string::npos) {
            std::string line = leftover.substr(0, pos);
            leftover.erase(0, pos + 2);

            std::cout << ">> " << line << std::endl;

            // Respond to PING
            if (line.compare(0, 4, "PING") == 0) {
                sendLine(sock, "PONG :" + line.substr(5));
            }

            // Respond to !hello command in channel
            size_t privmsg = line.find("PRIVMSG");
            size_t exclam = line.find("!");
            size_t hello = line.find("!hello");
            size_t ecco = line.find("!ecco");
            if (privmsg != std::string::npos && hello != std::string::npos) {
                std::string sender = "someone";
                if (exclam != std::string::npos && exclam > 1)
                    sender = line.substr(1, exclam - 1);

                size_t chan_start = line.find("PRIVMSG") + 8;
                size_t chan_end = line.find(" :", chan_start);
                std::string chan = line.substr(chan_start, chan_end - chan_start);

                sendLine(sock, "PRIVMSG " + chan + " :Hello, " + sender + "!");
            }
            // Respond to !ecco command in channel
            else if (privmsg != std::string::npos && ecco != std::string::npos) {
                // Extract channel name
                size_t chan_start = line.find("PRIVMSG") + 8;
                size_t chan_end = line.find(" :", chan_start);
                std::string chan = line.substr(chan_start, chan_end - chan_start);

                // Extract the message after !ecco
                size_t ecco_start = line.find("!ecco");
                size_t msg_start = ecco_start + 5; // length of "!ecco"
                // skip any space after !ecco
                if (line[msg_start] == ' ')
                    msg_start++;

                std::string echo_msg = line.substr(msg_start);

                // Only reply if there is something after !ecco
                if (!echo_msg.empty())
                    sendLine(sock, "PRIVMSG " + chan + " :" + echo_msg);
            }
        }
    }
    close(sock);
    return 0;
}