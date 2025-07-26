#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerSocket.hpp"
#include "ServerConfig.hpp"
#include "config.hpp"
#include <poll.h>

class ServerManager {
    private:
        std::vector<ServerSocket> _sockets;
        // list dyal poll file descriptors
        std::vector<struct pollfd> _pollFds;
        // sockets li kaynin f listen
        std::map<int, ServerConfig> _listeningSockets;
        // clients li ssefti request
        std::map<int, std::string> _clientRequests;
        // configs li merta9ta b client
        std::map<int, ServerConfig> _clientConfigs;
    
    public:
        ServerManager();
        ~ServerManager();

        void initServers(const std::vector<ServerConfig>& ServerConfig);
        std::vector<struct pollfd>& getPollFDs() const;
        void handlePollEvents();
        void receiveRequest(int clientFd);
        void sendResponse(int clientFd);
        void setPollFdToWrite(int fd);
        void acceptConnection(int serverFd, const ServerConfig& config);
        void removeFD(int fd);
};


#endif