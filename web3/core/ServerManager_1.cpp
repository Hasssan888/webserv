#include "ServerManager_1.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <cstdio>  // من أجل perror


ServerManager::ServerManager(const std::vector<ServerConfig>& configs) {
    for (size_t i = 0; i < configs.size(); ++i) {
        _serverSockets.push_back(ServerSocket(configs[i]));
    }

    for (size_t i = 0; i < _serverSockets.size(); ++i) {
        ServerSocket& sock = _serverSockets[i];
        _listeningSockets.insert(std::make_pair(sock.getFd(), &sock));
    }

    // إعداد pollfd للـ listening sockets
    for (size_t i = 0; i < _serverSockets.size(); ++i) {
        struct pollfd pfd;
        pfd.fd = _serverSockets[i].getFd();
        pfd.events = POLLIN;
        pfd.revents = 0;
        _pollFds.push_back(pfd);
    }
}


ServerManager::~ServerManager() {
    for (std::map<int, ClientConnection*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    _clients.clear();
}

void ServerManager::acceptConnection(int serverFd) {
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &addrLen);

    if (clientFd < 0) {
        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
        return;
    }

    // Set client socket to non-blocking
    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    // Add client fd to poll fds for reading
    struct pollfd pfd;
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollFds.push_back(pfd);

    // Create a new ClientConnection object and add it to the map
    _clients[clientFd] = new ClientConnection(clientFd);

    std::cout << "[DEBUG] New client accepted: " << clientFd << std::endl;
}

void ServerManager::handleClient(int clientFd) {
    ClientConnection* client = _clients[clientFd];
    if (!client->readFromSocket()) {
        std::cout << "[DEBUG] Client disconnected: " << clientFd << std::endl;
        close(clientFd);
        delete client;
        _clients.erase(clientFd);

        // Remove from poll fds
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].fd == clientFd) {
                _pollFds.erase(_pollFds.begin() + i);
                break;
            }
        }
        return;
    }

    if (client->isRequestComplete()) {
        std::string request = client->getRequest();
        std::cout << "[DEBUG] Request received:\n" << request << std::endl;

        client->buildResponse(request);

        // Change poll event to POLLOUT to send response
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].fd == clientFd) {
                _pollFds[i].events = POLLOUT;
                break;
            }
        }
    }
}

void ServerManager::run() {
    std::cout << "Server is running..." << std::endl;

    while (true) {
        int ret = poll(_pollFds.data(), _pollFds.size(), -1);
        if (ret < 0) {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < _pollFds.size(); ++i) {
            int fd = _pollFds[i].fd;

            if (_pollFds[i].revents & POLLIN) {
                // New connection on a listening socket
                if (_listeningSockets.count(fd) > 0) {
                    acceptConnection(fd);
                } else {
                    // Data from a connected client
                    handleClient(fd);
                }
            } else if (_pollFds[i].revents & POLLOUT) {
                // Ready to write to client
                ClientConnection* client = _clients[fd];
                if (client->writeToSocket()) {
                    std::cout << "[DEBUG] Response sent to client: " << fd << std::endl;
                    close(fd);
                    delete client;
                    _clients.erase(fd);
                    _pollFds.erase(_pollFds.begin() + i);
                    --i;
                }
            }
        }
    }
}
