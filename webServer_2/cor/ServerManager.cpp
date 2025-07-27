// ServerManager.cpp
#include "ServerManager.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>   
#include <sys/socket.h>   
#include <cstring>        
#include <stdexcept>
#include <cstdio>

ServerManager::ServerManager(const std::vector<ServerConfig>& configs) {
    // ✅ Fixed: Reserve space first to avoid reallocations
    _sockets.reserve(configs.size());
    _pollFds.reserve(configs.size() + 10); // Extra space for client connections
    
    for (size_t i = 0; i < configs.size(); ++i) {
        std::cout << "🔧 Creating server socket " << (i + 1) << "..." << std::endl;
        
        // Create socket and setup immediately
        ServerSocket sock(configs[i]);
        sock.setup();
        
        // Store the fd before moving the socket
        int server_fd = sock.getFd();
        
        // Add to our collections
        _sockets.push_back(sock);  // This will transfer ownership via copy constructor
        _pollFds.push_back((struct pollfd){ server_fd, POLLIN, 0 });
        
        std::cout << "✅ Server socket " << (i + 1) << " ready with fd: " << server_fd << std::endl;
    }
}

ServerManager::~ServerManager() {
    for (std::map<int, ClientConnection*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
}

void ServerManager::run() {
    std::cout << "🔄 Entering server loop..." << std::endl;
    std::cout << "🚀 Server is running with " << _sockets.size() << " server socket(s)!" << std::endl;
    
    while (true) {
        int ret = poll(&_pollFds[0], _pollFds.size(), -1);
        if (ret < 0) {
            perror("poll failed");
            throw std::runtime_error("poll failed");
        }

        // ✅ Fixed: Iterate backwards to safely remove elements
        for (int i = static_cast<int>(_pollFds.size()) - 1; i >= 0; --i) {
            int fd = _pollFds[i].fd;
            
            if (_pollFds[i].revents & POLLHUP) {
                std::cout << "🔌 Client disconnected: fd " << fd << std::endl;
                if (_clients.find(fd) != _clients.end()) {
                    delete _clients[fd];
                    _clients.erase(fd);
                }
                _pollFds.erase(_pollFds.begin() + i);
                continue;
            }

            if (_pollFds[i].revents & POLLERR) {
                std::cerr << "❌ Socket error on fd " << fd << std::endl;
                if (_clients.find(fd) != _clients.end()) {
                    delete _clients[fd];
                    _clients.erase(fd);
                }
                _pollFds.erase(_pollFds.begin() + i);
                continue;
            }

            // Handle events
            if (_pollFds[i].revents & POLLIN) {
                // Check if this is a server socket (not a client)
                const ServerSocket* serverSocket = NULL;
                for (size_t j = 0; j < _sockets.size(); ++j) {
                    if (_sockets[j].getFd() == fd) {
                        serverSocket = &_sockets[j];
                        break;
                    }
                }
                
                if (serverSocket) {
                    // New connection on server socket
                    sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int client_fd = accept(fd, (sockaddr*)&client_addr, &addrlen);
                    if (client_fd >= 0) {
                        std::cout << "📥 New client connected: fd " << client_fd << std::endl;
                        // ✅ Fixed: Pass the ServerConfig to ClientConnection
                        ClientConnection* client = new ClientConnection(client_fd, serverSocket->getConfig());
                        _clients[client_fd] = client;
                        _pollFds.push_back((struct pollfd){ client_fd, POLLIN, 0 });
                    } else {
                        perror("accept failed");
                    }
                } else {
                    // Existing client readable
                    if (_clients.find(fd) != _clients.end()) {
                        ClientConnection* client = _clients[fd];
                        client->readRequest();
                        if (client->isRequestComplete()) {
                            client->generateResponse();
                            _pollFds[i].events = POLLOUT; // switch to write mode
                        }
                    }
                }
                
            } else if (_pollFds[i].revents & POLLOUT) {
                // Client socket ready for writing
                if (_clients.find(fd) != _clients.end()) {
                    ClientConnection* client = _clients[fd];
                    client->sendResponse();

                    if (client->isDone()) {
                        std::cout << "✅ Done with client fd: " << fd << std::endl;
                        delete client;
                        _clients.erase(fd);
                        _pollFds.erase(_pollFds.begin() + i);
                    }
                }
            }
        }
    }
}