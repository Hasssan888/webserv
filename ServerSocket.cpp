#include "ServerSocket.hpp"
#include <cstring>

ServerSocket::ServerSocket(const std::string& host, int port)
    : _fd(-1), _host(host), _port(port) {}

ServerSocket::~ServerSocket() {
    if (_fd >= 0)
        close(_fd);
}

void ServerSocket::setup() {
    // 1.Create socket
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0)
        throw std::runtime_error("socket() faild");
    
    // 2. Set socket options (SO_REUSEADDR)
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt() faild");
    
    // 3. Create address structure
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _host.c_str(), &addr.sin_addr) <= 0)
        throw std::runtime_error("Invalid host IP address: " + _host);
    
    // 4. Bind socket to address
    if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind() faild");
    
    // 5. Set to non-blocking
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl() faild to set O_NONBLOCK");

    // 6. Listen for connections
    if (listen(_fd, SOMAXCONN) < 0)
        throw std::runtime_error("listen() faild");

    std::cout << "Listening on " << _host << ":" << _port << std::endl;
}

int ServerSocket::getFd() const {
    return _fd;
}