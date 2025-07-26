#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "ServerConfig.hpp"
#include <string>
#include <stdexcept>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_pton
#include <fcntl.h>      // fcntl
#include <unistd.h>     // close
#include <sys/socket.h> // socket, bind, listen, setsockopt


class ServerSocket {
    private:
        int _fd;
        std::string _host;
        int _port;

    public:
        ServerSocket(const std::string& host, int port);
        ~ServerSocket();

        int getFd() const;
        void setup();
};

#endif