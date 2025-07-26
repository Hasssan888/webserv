#ifndef SERVERSOCKET_1_HPP
#define SERVERSOCKET_1_HPP

#include "../config/ConfigParser.hpp"

#include <netinet/in.h>   // ✅ هذا هو الصحيح (in.h مش int.h)

#include <string>

class ServerSocket {
    private:
        int _fd;
        ServerConfig _config;
        struct sockaddr_in _addr;
    
    public:
        ServerSocket(const ServerConfig& config);
        ~ServerSocket();

        int getFd() const;
        const ServerConfig& getConfig() const;

        void setupSocket();
};


#endif