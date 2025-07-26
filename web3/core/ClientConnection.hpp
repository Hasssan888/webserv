#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include <sys/socket.h>

class ClientConnection {
private:
    int _fd;
    std::string _readBuffer;
    std::string _writeBuffer;
    bool _requestComplete;

public:
    ClientConnection(int fd);
    ~ClientConnection();

    int getFd() const;

    bool readFromSocket();
    bool writeToSocket();

    bool isRequestComplete() const;
    std::string getRequest() const;
    void buildResponse(const std::string& request);
};

#endif
