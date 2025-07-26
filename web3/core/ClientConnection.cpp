#include "ClientConnection.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>

ClientConnection::ClientConnection(int fd)
    : _fd(fd), _requestComplete(false) {}

ClientConnection::~ClientConnection() {
    close(_fd);
}

int ClientConnection::getFd() const {
    return _fd;
}

bool ClientConnection::readFromSocket() {
    char buffer[4096];
    ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0)
        return false;

    _readBuffer.append(buffer, bytesRead);

    // simple condition: if "\r\n\r\n" exists, we assume request is complete
    if (_readBuffer.find("\r\n\r\n") != std::string::npos)
        _requestComplete = true;

    return true;
}

bool ClientConnection::writeToSocket() {
    ssize_t bytesSent = send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);

    if (bytesSent < 0)
        return false;

    _writeBuffer.erase(0, bytesSent);
    return _writeBuffer.empty();
}

bool ClientConnection::isRequestComplete() const {
    return _requestComplete;
}

std::string ClientConnection::getRequest() const {
    return _readBuffer;
}

void ClientConnection::buildResponse(const std::string& request) {
    (void)request;
    _writeBuffer = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
}
