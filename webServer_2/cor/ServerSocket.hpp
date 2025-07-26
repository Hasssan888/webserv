// ServerSocket.hpp
#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "../config/ServerConfig.hpp"
#include <string>

class ServerSocket {
    private:
        int _fd;
        ServerConfig _config;

    public:
        ServerSocket(const ServerConfig& config);
        ~ServerSocket();

        int getFd() const;
        const ServerConfig& getConfig() const;

        void setup(); // socket, bind, listen
};

#endif
