
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

std::vector<struct pollfd>& ServerManager::getPollFDs() const {
    return _pollfds;
}

void ServerManager::handlePollEvents() {
    
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        struct pollfd& pfd = _pollFds[i];

        if (pfd.revents & POLLIN) {
            if (_listeningSockets.count(pfd.fd)) {
                const ServerConfig& conf = _listeningSockets[pfd.fd];
                acceptConnection(pfd.fd, conf);
            } else {
                receiveRequest(pfd.fd);
            }
        } else if (pfd.revents & POLLOUT) {
            sendResponse(pfd.fd);
        } else if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            close(pfd.fd);
            removeFD(pfd.fd);
        }
    }
}

void ServerManager::receiveRequest(int clientFd) {
    char buffer[4096];
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        // Clinet sda la connexion aw wa9a error
        close(clientFd);
        removeFD(clientFd);
        return;
    }

    buffer[bytesRead] = '\0';
    std::string request(buffer);

    std::cout << "Received request from fd" << clientFd << ":\n" 
        << request << std::endl;
    
    _clientRequests[clientFd];
    setPollFdToWrite(clientFd);
}

void ServerManager::sendResponse(int clientFd) {
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";

        ssize_t sent = send(clientFd, response.c_str(), response.size(), 0);

        if (sent == -1) {
            perror("send failed");
            close(clientFd);
            removeFD(clientFd);
            return;
        }

        std::cout << "Sent response to fd " << clientFd << std::endl;
        close(clientFd);
        removeFD(clientFd);
}

void ServerManager::setPollFdToWrite(int fd) {
    for (size_t i = 0; i < _pollFds.size(); i++) {
        if (_pollFds[i].fd == fd) {
            _pollFds[i].events = POLLOUT;
            break;
        }
    }
}

void ServerManager::acceptConnection(int serverFd, const ServerConfig& config) {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(serverFd, (sockaddr*)&clientAddr, &clientLen);

    if (clientFd == -1) {
        perror("accept failed");
        return;
    }

    std::cout << "Accepted connection on fd " << clientFd << std::endl;

    // Zid client fd l pooll
    struct pollfd fd;
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    pfd.revernts = 0;
    _pollFds.push_back(pfd);

    // T9der t7taj tkhazen chi data 3la client fd (config, request...)
    _clientConfigs[clientFd] = config;
}

void ServerManager::removeFD(int fd) {
    // Remove from pollFds
    for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        if (it->fd == fd) {
            _pollFds.erase(it);
            break;
        }
    }

    // Remove from other maps
    _clientRequests.erase(fd);
    _clientConfigs.erase(fd);
}