#include "ClientConnection.hpp"
#include <unistd.h>
#include "../Http/HttpRequest.hpp"
#include "../Http/HttpResponse.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

ClientConnection::ClientConnection(int fd) : _fd(fd), _requestComplete(false), _responseReady(false) {}

ClientConnection::~ClientConnection() {
    if (_fd != -1)
        close(_fd);
}

int ClientConnection::getFd() const {
    return _fd;
}

void ClientConnection::generateResponse() {
    std::cout << "[DEBUG] Parsing request..." << std::endl;
    if (!_request.parse(_readBuffer)) {
        std::cout << "[DEBUG] Failed to parse request -> 400 Bad Request" << std::endl;
        HttpResponse response;
        response.setStatusCode(400);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("400 Bad Request");
        _writeBuffer = response.toString();
        _responseReady = true;
        return;
    }

    std::string uri = _request.getUri();
    std::string method = _request.getMethod();

    std::cout << "[DEBUG] Request URI: " << uri << std::endl;
    std::cout << "[DEBUG] Request Method: " << method << std::endl;

    // البحث على location المناسبة
    const std::map<std::string, LocationConfig>& locations = _serverConfig.getLocations();
    const LocationConfig* matchedLocation = NULL;
    size_t longestMatch = 0;

    for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const std::string& path = it->first;
        if (uri.find(path) == 0 && path.length() > longestMatch) {
            matchedLocation = &it->second;
            longestMatch = path.length();
        }
    }

    if (!matchedLocation) {
        std::cout << "[DEBUG] No matching location found -> 404 Not Found" << std::endl;
        HttpResponse response;
        response.setStatusCode(404);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("404 Not Found");
        _writeBuffer = response.toString();
        _responseReady = true;
        return;
    }

    std::cout << "[DEBUG] Matched location root: " << matchedLocation->getRoot() << std::endl;

    // التحقق من صلاحية الطريقة
    if (matchedLocation->getAllowedMethods().find(method) == 
            matchedLocation->getAllowedMethods().end())  {
        HttpResponse response;
        response.setStatusCode(405);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("405 Method Not Allowed");
        _writeBuffer = response.toString();
        _responseReady = true;
        return;
    }

    // بناء المسار الكامل
    std::string root = matchedLocation->getRoot();
    std::string filePath = root + uri.substr(longestMatch);

    if (!filePath.empty() && filePath[filePath.size() - 1] == '/')
        filePath += _serverConfig.getIndex();

    std::cout << "[DEBUG] Full file path: " << filePath << std::endl;

    HttpResponse response;

    if (method == "GET") {
        std::ifstream file(filePath.c_str());
        if (file) {
            std::stringstream ss;
            ss << file.rdbuf();
            response.setStatusCode(200);
            response.setHeader("Content-Type", "text/html"); // ممكن تحسنها حسب الامتداد
            response.setBody(ss.str());
            std::cout << "[DEBUG] GET: File found, sending 200 OK" << std::endl;
        } else {
            response.setStatusCode(404);
            response.setHeader("Content-Type", "text/plain");
            response.setBody("404 Not Found");
            std::cout << "[DEBUG] GET: File not found -> 404 Not Found" << std::endl;
        }
    }
    // تقدر تزيد دعم POST, DELETE, PUT هنا

    else {
        response.setStatusCode(501);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("501 Not Implemented");
        std::cout << "[DEBUG] Method " << method << " not implemented -> 501 Not Implemented" << std::endl;
    }

    _writeBuffer = response.toString();
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
        
        // analyes request
        if (_request.parse(_readBuffer)) {
            std::cout << "Method: " << _request.getMethod() << std::endl;
            std::cout << "URI: " << _request.getUri() << std::endl;
        } else {
            std::cerr << "Failed to parse HTTP request" << std::endl;
        }
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