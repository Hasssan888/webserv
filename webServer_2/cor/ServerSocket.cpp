#include "ServerSocket.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>   
#include <cstring>
#include <iostream>
#include <cstdio>        
#include <stdexcept>
#include <errno.h>

ServerSocket::ServerSocket(const ServerConfig& config) : _fd(-1), _config(config) {}

// ✅ Copy constructor - transfers ownership
ServerSocket::ServerSocket(const ServerSocket& other) : _fd(other._fd), _config(other._config) {
    // Transfer ownership - the original object should not close this fd
    const_cast<ServerSocket&>(other)._fd = -1;
}

// ✅ Assignment operator - transfers ownership
ServerSocket& ServerSocket::operator=(const ServerSocket& other) {
    if (this != &other) {
        // Close current fd if we have one
        if (_fd != -1) {
            close(_fd);
        }
        
        // Transfer ownership
        _fd = other._fd;
        _config = other._config;
        const_cast<ServerSocket&>(other)._fd = -1;
    }
    return *this;
}

ServerSocket::~ServerSocket() {
    if (_fd != -1) {
        std::cout << "🔌 Closing socket fd: " << _fd << std::endl;
        close(_fd);
    }
}

int ServerSocket::getFd() const {
    return _fd;
}

const ServerConfig& ServerSocket::getConfig() const {
    return _config;
}

void ServerSocket::setup() {
    std::cout << "🔧 Setting up socket for " << _config.getHost() << ":" << _config.getPort() << std::endl;
    
    // Create socket
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
        std::cerr << "❌ socket() failed: " << strerror(errno) << std::endl;
        throw std::runtime_error("Failed to create socket");
    }
    std::cout << "✅ Socket created with fd: " << _fd << std::endl;

    // Set socket options
    int yes = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        std::cerr << "❌ setsockopt(SO_REUSEADDR) failed: " << strerror(errno) << std::endl;
        close(_fd);
        _fd = -1;  // ✅ Mark as invalid
        throw std::runtime_error("setsockopt failed");
    }
    std::cout << "✅ SO_REUSEADDR set" << std::endl;

    // Prepare address structure
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_config.getPort());

    std::cout << "🔍 Host from config: '" << _config.getHost() << "'" << std::endl;
    
    // Set IP address
    if (_config.getHost() == "127.0.0.1") {
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        std::cout << "✅ Using localhost (127.0.0.1)" << std::endl;
    } else if (_config.getHost() == "0.0.0.0" || _config.getHost().empty()) {
        addr.sin_addr.s_addr = INADDR_ANY;
        std::cout << "✅ Using INADDR_ANY (all interfaces)" << std::endl;
    } else {
        addr.sin_addr.s_addr = inet_addr(_config.getHost().c_str());
        if (addr.sin_addr.s_addr == INADDR_NONE) {
            std::cerr << "❌ Invalid IP address: " << _config.getHost() << std::endl;
            close(_fd);
            _fd = -1;
            throw std::runtime_error("Invalid IP address");
        }
        std::cout << "✅ Using custom IP: " << _config.getHost() << std::endl;
    }

    // Bind socket
    std::cout << "🔗 Attempting to bind to " << _config.getHost() << ":" << _config.getPort() << std::endl;
    if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "❌ bind() failed: " << strerror(errno) << std::endl;
        std::cerr << "   Trying to bind to: " << _config.getHost() << ":" << _config.getPort() << std::endl;
        std::cerr << "   Socket fd: " << _fd << std::endl;
        close(_fd);
        _fd = -1;
        throw std::runtime_error("bind failed");
    }
    std::cout << "✅ Successfully bound to port " << _config.getPort() << std::endl;

    // Set non-blocking
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "❌ fcntl(O_NONBLOCK) failed: " << strerror(errno) << std::endl;
        close(_fd);
        _fd = -1;
        throw std::runtime_error("fcntl failed");
    }
    std::cout << "✅ Socket set to non-blocking mode" << std::endl;

    // Start listening
    if (listen(_fd, 100) < 0) {
        std::cerr << "❌ listen() failed: " << strerror(errno) << std::endl;
        close(_fd);
        _fd = -1;
        throw std::runtime_error("listen failed");
    }
    
    std::cout << "🎧 Successfully listening on " << _config.getHost() << ":" << _config.getPort() << std::endl;
    std::cout << "📡 Server socket ready with fd: " << _fd << std::endl;
}