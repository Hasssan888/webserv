#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerSocket_1.hpp"
#include "../config/ServerConfig_1.hpp"
#include "ClientConnection.hpp"
#include <vector>
#include <poll.h>
#include <map>

class ServerManager {
private:
    std::vector<ServerSocket> _serverSockets;
    std::vector<struct pollfd> _pollFds;
    std::map<int, ClientConnection*> _clients;
    std::map<int, ServerSocket*> _listeningSockets;

    void acceptConnection(int serverFd);
    void handleClient(int clientFd);

public:
    ServerManager(const std::vector<ServerConfig>& configs);
    ~ServerManager();
    void run();
};

#endif
