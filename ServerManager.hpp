#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerSocket.hpp"
#include "ServerConfig.hpp"
#include "config.hpp"
#include <poll.h>

class ServerManager {
    private:
        std::vector<ServerSocket> _sockets;
        std::vector<struct pollfd> _pollFds;
        std::map<int, ServerConfig> _listeningSockets;
    
    public:
        ServerManager();
        ~ServerManager();

        void initServers(const std::vector<ServerConfig>& ServerConfig);
        const std::vector<struct pollfd>& getPollFDs() const;
        void handlePollEvents();
};


#endif