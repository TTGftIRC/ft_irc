#include "../../inc/Server.hpp"

void Server::requestPollOut(int client_fd, bool enable) {
    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
        if (it->fd == client_fd) {
            if (enable) { 
                it->events |= POLLOUT;
            } else {
                it->events &= ~POLLOUT;
            }
            break;
        }
    }
}

int Server::listenPoll(struct pollfd *fds, nfds_t nfds, int timeout){ 
    int poll_ret = 0;
    while (true){
        poll_ret = poll(fds, nfds, timeout);
        if (poll_ret == -1 && errno == EINTR){
            continue;
        }
        else if (poll_ret == -1){
            std::cerr << "Error: poll has failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        else
            break;
    }
    return poll_ret;
}

void Server::AddToPollStrct(int new_socket, sockaddr_in client_addr){
    _makeNonBlock(new_socket);
    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    Client* new_client = new Client(new_socket, client_ip, this);
    _clients.insert(std::make_pair(new_socket, new_client));
    pollfd new_conexion;
    new_conexion.fd = new_socket;
    new_conexion.events = POLLIN;
    new_conexion.revents = 0;
    _poll_fds.push_back(new_conexion);
}

int Server::handleNewServConnect(){
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int new_socket = accept(_poll_fds[0].fd, reinterpret_cast<sockaddr *>(&client_addr), &addr_len);
    if (new_socket == -1){
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return (-1);
        else{
            std::cerr << "Error: accept has failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    AddToPollStrct(new_socket, client_addr);
    return(42);
}

bool Server::RecvData(int i, Client *curr){
    char buffer[1024] = {0};
    ssize_t bytes_read = recv(_poll_fds[i].fd, buffer, sizeof(buffer), 0);
    if (bytes_read > 0) {
        // std::cout << "recv data: " << buffer << std::endl;
        curr->appendRecvData(buffer, bytes_read);
        std::string cmd;
        while (!(cmd = curr->extractLineFromRecv()).empty())
            _handleClientMessage(*this, curr, cmd);
        return true;
    } else if (bytes_read == 0) {
        std::cout << "Client has been disconnected !" << std::endl;
        CleanClient(i);
        return false;
    } else if(bytes_read == -1){
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return true;
        else{
            CleanClient(i);
            return false;       
        }
    }
    return true;
}

bool Server::SendData(int i, Client *curr){
    if (!curr->hasData()) {
        return true;
    }
    std::string& data_to_send = curr->getSendBuf();

    // std::cout << "curr->getSendBuf(): " << curr->getSendBuf() << std::endl;

    ssize_t bytes = send(_poll_fds[i].fd, data_to_send.data(), data_to_send.size(), 0);

    // if (bytes > 0) {
    //     std::cout << "sent " << bytes << " bytes: " << data_to_send.substr(0, bytes) << std::endl;
    // }

    if (bytes == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return true;
        }
        std::cerr << "Could not send data" << std::endl;
        CleanClient(i);
        return false;
    }

    curr->helpSenderEvent(bytes);

    return true;
}

void Server::HandlePollREvents(){
    size_t i = 1;
    while(i < _poll_fds.size()) {
        Client* curr = _clients[_poll_fds[i].fd];
        if (_poll_fds[i].revents & (POLLHUP | POLLNVAL | POLLERR)) {
            std::cout << "Client has been disconnected !" << std::endl;
            CleanClient(i);
            continue;
        } if (_poll_fds[i].revents & POLLIN) {
            if (!RecvData(i, curr))
            continue;
        } if (_poll_fds[i].revents & POLLOUT) {
            // std::cout << "DEBUG: POLLOUT detected for client FD: " << _poll_fds[i].fd << std::endl;
            if (!SendData(i, curr))
            continue;                
        }
        ++i;
    }
}

void Server::runPoll() {
    pollfd server_fd;
    server_fd.fd = _listening_socket;
    server_fd.events = POLLIN;
    server_fd.revents = 0;
    _poll_fds.push_back(server_fd); // first elem of the pollfd will be the server which will be waiting for new events
    while (!sig_recvied) {
        listenPoll(_poll_fds.data(), _poll_fds.size(), -1);
        if (_poll_fds[0].revents & POLLIN) {
            if (_poll_fds[0].fd == _listening_socket) {
                // handle new client conexions
                if(handleNewServConnect() == -1)
                    continue;
            }
        }
        HandlePollREvents();
    }
}
