// ServerManager.hpp
#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerSocket.hpp"
#include "ClientConnection.hpp"
#include <vector>
#include <map>  // ✅ Added missing include
#include <poll.h>

class ServerManager {
    private:
        std::vector<ServerSocket> _sockets;
        std::vector<struct pollfd> _pollFds;
        std::map<int, ClientConnection*> _clients; // client fd -> pointer

    public:
        ServerManager(const std::vector<ServerConfig>& configs);
        ~ServerManager();
        
        void run(); // main event loop
};

#endif