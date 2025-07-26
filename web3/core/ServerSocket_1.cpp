#include "ServerSocket_1.hpp"
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>


ServerSocket::ServerSocket(const ServerConfig& config) : _config(config) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0)
        throw std::runtime_error("Faild to create socket");
    
    int opt = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    fcntl(_fd, F_SETFL, O_NONBLOCK);

    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(config.getPort());
    _addr.sin_addr.s_addr = inet_addr(config.getHost().c_str());

    if (bind(_fd, (struct sockaddr*)&_addr, sizeof(_addr)) < 0)
        throw std::runtime_error("Failed to bind socket");
    
    if (listen(_fd, 128) < 0)
        throw std::runtime_error("Failed to bind listen");
}

ServerSocket::~ServerSocket() {
    close(_fd);
}

int ServerSocket::getFd() const {
    return _fd;
}

const ServerConfig& ServerSocket::getConfig() const {
    return _config;
}
