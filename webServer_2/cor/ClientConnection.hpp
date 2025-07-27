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
    const ServerConfig* _serverConfig;  // ✅ Fixed: pointer to avoid copying

public:
    ClientConnection(int fd, const ServerConfig& config);  // ✅ Fixed: pass config
    ~ClientConnection();

    int getFd() const;

    bool isRequestComplete() const;
    bool isResponseReady() const;

    void readRequest();
    void generateResponse(); 
    void sendResponse();

    bool isDone() const;
};

#endif