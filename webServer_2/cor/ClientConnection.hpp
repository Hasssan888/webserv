#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../Http/HttpRequest.hpp"
#include "../config/ServerConfig.hpp"

class ClientConnection {
private:
    int _fd;                        // File descriptor ديال client socket
    std::string _readBuffer;       // باش نجمع request لي جا من client
    std::string _writeBuffer;      // الرد لي غادي نسيفطو
    bool _requestComplete;         // واش سالا request
    bool _responseReady;   
    HttpRequest _request;        // واش وجدنا response
    ServerConfig _serverConfig;
public:
    ClientConnection(int fd);
    ~ClientConnection();

    int getFd() const;

    bool isRequestComplete() const;
    bool isResponseReady() const;

    void readRequest();
    void generateResponse(); // لاحقاً نقدر نمررو ServerConfig etc
    void sendResponse();

    bool isDone() const; // واش نقدر نحيدو من poll
};





#endif