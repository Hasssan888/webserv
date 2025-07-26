#include "ServerSocket.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>   // <<<<< هادي إضافية ضرورية
#include <cstring>
#include <iostream>
#include <cstdio>  // لتعريف perror



ServerSocket::ServerSocket(const ServerConfig& config) : _fd(-1), _config(config) {}

ServerSocket::~ServerSocket() {
    if (_fd != -1)
        close(_fd);
}

int ServerSocket::getFd() const {
    return _fd;
}

const ServerConfig& ServerSocket::getConfig() const {
    return _config;
}

void ServerSocket::setup() {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
        perror("socket failed");
        throw std::runtime_error("Failed to create socket");
    }
    std::cout << "Socket FD created: " << _fd << std::endl;

    int yes = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("setsockopt failed");
        throw std::runtime_error("setsockopt failed");
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_config.getPort());

    std::cout << "_config.getHost(): " << _config.getHost() << std::endl;
    // Listen on all interfaces:
    addr.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Binding to port: " << _config.getPort() << " on all interfaces" << std::endl;
    std::cout << "⚠️ About to call bind() on fd " << _fd << std::endl;
    if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        throw std::runtime_error("bind failed");
    }

    std::cout << "After bind -> fd: " << _fd << std::endl;

    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl failed");
        throw std::runtime_error("fcntl failed");
    }

    if (listen(_fd, 100) < 0) {
        perror("listen failed");
        throw std::runtime_error("listen failed");
    }
    std::cout << "After listen -> fd: " << _fd << std::endl;

    std::cout << "✅ Listening on all interfaces:" << _config.getPort() << std::endl;
}
