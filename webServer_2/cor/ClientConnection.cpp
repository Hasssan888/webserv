#include "ClientConnection.hpp"
#include <unistd.h>

ClientConnection::ClientConnection(int fd) : _fd(fd), _requestComplete(false), _responseReady(false) {}

ClientConnection::~ClientConnection() {
    if (_fd != -1)
        close(_fd);
}

int ClientConnection::getFd() const {
    return _fd;
}

void ClientConnection::generateResponse() {
    _writeBuffer = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, world!";
    _responseReady = true;
}

void ClientConnection::readRequest() {
    char buffer[1024];
    ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        _requestComplete = true; // نعتبروه سالا أو تعطّل
        return;
    }
    buffer[bytesRead] = '\0';
    _readBuffer += buffer;

    if (_readBuffer.find("\r\n\r\n") != std::string::npos) {
        _requestComplete = true; // نهاية الهيدر
    }
}

void ClientConnection::sendResponse() {
    if (_writeBuffer.empty())
        return;

    ssize_t bytesSent = send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);
    if (bytesSent <= 0)
        return;

    _writeBuffer.erase(0, bytesSent);
    if (_writeBuffer.empty())
        close(_fd); // نغلق الاتصال من بعد الرد
}

bool ClientConnection::isDone() const {
    return _requestComplete && _writeBuffer.empty();
}

bool ClientConnection::isRequestComplete() const {
    return _requestComplete;  // ✅ Fixed: return actual state instead of always true
}

// ✅ Added missing method
bool ClientConnection::isResponseReady() const {
    return _responseReady;
}