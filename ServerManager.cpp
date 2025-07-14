
#include "ServerManager.hpp"


ServerManager::ServerManager() {}

ServerManager::~ServerManager() {}

void ServerManager::initServers(const std::vector<ServerConfig>& serverConfigs) {
    for (size_t i = 0; i < serverConfigs.size(); i++) {
        try
        {
            const ServerConfig& conf = serverConfigs[i];
            ServerSocket sock(conf.host, conf.port);
            sock.setup();
            _sockets.push_back(sock);

            struct pollfd pfd;
            pfd.fd = sock.getFd();
            pfd.events = POLLIN;
            pfd.revents = 0;
            _pollFds.push_back(pfd);

        }
        catch(const std::exception& e)
        {
           std::cerr << "Failed to setup server " << serverConfigs[i].host << ":" << serverConfigs[i].port
                      << " → " << e.what() << std::endl;
        }
        
    }
}

const std::vector<struct pollfd>& ServerManager::getPollFDs() const {
    return _pollfds;
}

void ServerManager::handlePollEvents() {
    
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        struct pollfd& pfd = _pollFds[i];

        if (pfd.revents & POLLIN) {
            
        } else if (pfd.revents & POLLOUT) {

        } else if (pfd.revents & (POLLERR | POLLHUP | PLLNVAL)) {

        }
    }
}