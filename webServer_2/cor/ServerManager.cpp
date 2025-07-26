// ServerManager.cpp
#include "ServerManager.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>   // ✅ ضروري لـ sockaddr_in
#include <sys/socket.h>   // ✅ ضروري لـ accept()
#include <cstring>        // ✅ optional: memset


ServerManager::ServerManager(const std::vector<ServerConfig>& configs) {
    for (size_t i = 0; i < configs.size(); ++i) {
        ServerSocket sock(configs[i]);
        sock.setup();
        _pollFds.push_back((struct pollfd){ sock.getFd(), POLLIN, 0 });
        _sockets.push_back(sock);
    }
}

ServerManager::~ServerManager() {
    for (std::map<int, ClientConnection*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
}

void ServerManager::run() {
    std::cout << "🔄 Entering server loop..." << std::endl;
    std::cout << "🚀 Server is running...\n";
    while (true) {
        // std::cout << "🌀 Polling..." << std::endl;
        int ret = poll(&_pollFds[0], _pollFds.size(), -1);
        if (ret < 0)
            throw std::runtime_error("poll failed");

        for (size_t i = 0; i < _pollFds.size(); ++i) {
            // std::cout << "pollfd[" << i << "] = " << _pollFds[i].fd
            //   << ", events = " << _pollFds[i].events
            //   << ", revents = " << _pollFds[i].revents << std::endl;
            int fd = _pollFds[i].fd;
            
            if (_pollFds[i].revents & POLLHUP) {
                    std::cout << "🔌 Client disconnected: fd " << fd << std::endl;
                    if (_clients.find(fd) != _clients.end()) {
                        delete _clients[fd];
                        _clients.erase(fd);
                    }
                    _pollFds.erase(_pollFds.begin() + i);
                    --i;  // Adjust index because we removed an element
                    continue;
            }

            if (_pollFds[i].revents & POLLERR) {
                std::cerr << "❌ Socket error on fd " << fd << std::endl;
                if (_clients.find(fd) != _clients.end()) {
                    delete _clients[fd];
                    _clients.erase(fd);
                }
                _pollFds.erase(_pollFds.begin() + i);
                --i;
                continue;
            }

            // New connection
            if (_pollFds[i].revents & POLLIN) {
                if (_clients.find(fd) == _clients.end()) {
                    sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int client_fd = accept(fd, (sockaddr*)&client_addr, &addrlen);
                    if (client_fd >= 0) {
                        std::cout << "📥 New client: fd " << client_fd << std::endl;
                        ClientConnection* client = new ClientConnection(client_fd);
                        _clients[client_fd] = client;
                        _pollFds.push_back((struct pollfd){ client_fd, POLLIN, 0 });
                    }
                }
                // Existing client readable
                else {
                    ClientConnection* client = _clients[fd];
                    client->readRequest();
                    if (client->isRequestComplete()) {
                        client->generateResponse(); // existing response
                        _pollFds[i].events = POLLOUT; // switch to write 
                    }
                }
                
            } else if (_pollFds[i].revents & POLLOUT) {
                ClientConnection* client = _clients[fd];
                client->sendResponse();

                if (client->isDone()) {
                    std::cout << "✅ Done with client fd: " << fd << std::endl;
                    delete client;
                    _clients.erase(fd);
                    _pollFds.erase(_pollFds.begin() + i);
                    --i;                   
                }
            }
        }
    }
}
