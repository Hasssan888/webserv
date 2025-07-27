#include "ClientConnection.hpp"
#include <unistd.h>
#include "../Http/HttpRequest.hpp"
#include "../Http/HttpResponse.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

ClientConnection::ClientConnection(int fd, const ServerConfig& config) 
    : _fd(fd), _requestComplete(false), _responseReady(false), _serverConfig(&config) {}

ClientConnection::~ClientConnection() {
    if (_fd != -1)
        close(_fd);
}

int ClientConnection::getFd() const {
    return _fd;
}

void ClientConnection::generateResponse() {
    std::cout << "[DEBUG] ===== Generating Response =====" << std::endl;
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

    std::cout << "[DEBUG] Request URI: '" << uri << "'" << std::endl;
    std::cout << "[DEBUG] Request Method: '" << method << "'" << std::endl;

    // البحث على location المناسبة
    const std::map<std::string, LocationConfig>& locations = _serverConfig->getLocations();
    const LocationConfig* matchedLocation = NULL;
    std::string matchedPath;
    size_t longestMatch = 0;

    std::cout << "[DEBUG] Available locations:" << std::endl;
    for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        std::cout << "[DEBUG]   - Location: '" << it->first << "' -> Root: '" << it->second.getRoot() << "'" << std::endl;
        const std::string& path = it->first;
        
        // ✅ Fixed: Better matching logic
        if ((uri == path) || (uri.find(path) == 0 && (path == "/" || uri[path.length()] == '/' || uri.length() == path.length()))) {
            if (path.length() > longestMatch) {
                matchedLocation = &it->second;
                matchedPath = path;
                longestMatch = path.length();
                std::cout << "[DEBUG]   ✅ Match found for '" << path << "'" << std::endl;
            }
        }
    }

    if (!matchedLocation) {
        std::cout << "[DEBUG] No matching location found -> 404 Not Found" << std::endl;
        HttpResponse response;
        response.setStatusCode(404);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("404 Not Found - No matching location");
        _writeBuffer = response.toString();
        _responseReady = true;
        return;
    }

    std::cout << "[DEBUG] Matched location '" << matchedPath << "' with root: '" << matchedLocation->getRoot() << "'" << std::endl;

    // التحقق من صلاحية الطريقة
    const std::set<std::string>& allowedMethods = matchedLocation->getAllowedMethods();
    std::cout << "[DEBUG] Allowed methods: ";
    for (std::set<std::string>::const_iterator mit = allowedMethods.begin(); mit != allowedMethods.end(); ++mit) {
        std::cout << "'" << *mit << "' ";
    }
    std::cout << std::endl;

    if (allowedMethods.find(method) == allowedMethods.end()) {
        std::cout << "[DEBUG] Method '" << method << "' not allowed -> 405 Method Not Allowed" << std::endl;
        HttpResponse response;
        response.setStatusCode(405);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("405 Method Not Allowed");
        _writeBuffer = response.toString();
        _responseReady = true;
        return;
    }

    // ✅ بناء المسار الكامل - Fixed logic
    std::string root = matchedLocation->getRoot();
    std::string filePath;
    
    std::cout << "[DEBUG] Building file path..." << std::endl;
    std::cout << "[DEBUG]   Root: '" << root << "'" << std::endl;
    std::cout << "[DEBUG]   Matched path: '" << matchedPath << "'" << std::endl;
    std::cout << "[DEBUG]   URI: '" << uri << "'" << std::endl;
    
    // Get the remaining part of the URI after the matched location
    std::string remainingPath;
    if (uri.length() > matchedPath.length()) {
        remainingPath = uri.substr(matchedPath.length());
        // Remove leading slash if present and location path doesn't end with slash
        if (!remainingPath.empty() && remainingPath[0] == '/' && matchedPath != "/") {
            remainingPath = remainingPath.substr(1);
        }
    }
    
    std::cout << "[DEBUG]   Remaining path: '" << remainingPath << "'" << std::endl;
    
    // Start with root
    filePath = root;
    
    // Add trailing slash to root if needed
    if (!filePath.empty() && filePath[filePath.size() - 1] != '/' && !remainingPath.empty()) {
        filePath += "/";
    }
    
    // If remaining path is empty, we need index file
    if (remainingPath.empty()) {
        std::string indexFile = matchedLocation->getIndex();
        if (indexFile.empty()) {
            indexFile = _serverConfig->getIndex();
        }
        std::cout << "[DEBUG]   Using index file: '" << indexFile << "'" << std::endl;
        
        if (!filePath.empty() && filePath[filePath.size() - 1] != '/') {
            filePath += "/";
        }
        filePath += indexFile;
    } else {
        filePath += remainingPath;
    }

    std::cout << "[DEBUG] Final file path: '" << filePath << "'" << std::endl;
    
    // Check if file exists and is readable
    std::ifstream testFile(filePath.c_str());
    if (testFile.is_open()) {
        testFile.close();
        std::cout << "[DEBUG] ✅ File exists and is readable" << std::endl;
    } else {
        std::cout << "[DEBUG] ❌ File does not exist or is not readable" << std::endl;
    }

    HttpResponse response;

    if (method == "GET") {
        std::ifstream file(filePath.c_str());
        if (file.is_open()) {
            std::stringstream ss;
            ss << file.rdbuf();
            std::string content = ss.str();
            
            response.setStatusCode(200);
            
            // Simple content type detection
            if (filePath.find(".html") != std::string::npos || filePath.find(".htm") != std::string::npos) {
                response.setHeader("Content-Type", "text/html");
            } else if (filePath.find(".css") != std::string::npos) {
                response.setHeader("Content-Type", "text/css");
            } else if (filePath.find(".js") != std::string::npos) {
                response.setHeader("Content-Type", "application/javascript");
            } else {
                response.setHeader("Content-Type", "text/plain");
            }
            
            response.setBody(content);
            std::cout << "[DEBUG] ✅ GET: File loaded successfully (" << content.length() << " bytes) -> 200 OK" << std::endl;
        } else {
            response.setStatusCode(404);
            response.setHeader("Content-Type", "text/plain");
            response.setBody("404 Not Found - File not found: " + filePath);
            std::cout << "[DEBUG] ❌ GET: File not found '" << filePath << "' -> 404 Not Found" << std::endl;
        }
    }
    // You can add support for POST, DELETE, PUT here
    else {
        response.setStatusCode(501);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("501 Not Implemented");
        std::cout << "[DEBUG] Method " << method << " not implemented -> 501 Not Implemented" << std::endl;
    }

    _writeBuffer = response.toString();
    _responseReady = true;
    std::cout << "[DEBUG] ===== Response Ready =====" << std::endl;
}

void ClientConnection::readRequest() {
    char buffer[1024];
    ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        _requestComplete = true;
        return;
    }
    buffer[bytesRead] = '\0';
    _readBuffer += buffer;

    if (_readBuffer.find("\r\n\r\n") != std::string::npos) {
        _requestComplete = true;
        
        // Parse request for debugging
        if (_request.parse(_readBuffer)) {
            std::cout << "[DEBUG] Raw request parsed - Method: " << _request.getMethod() 
                      << ", URI: " << _request.getUri() << std::endl;
        } else {
            std::cerr << "[ERROR] Failed to parse HTTP request" << std::endl;
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
        close(_fd);
}

bool ClientConnection::isDone() const {
    return _requestComplete && _writeBuffer.empty();
}

bool ClientConnection::isRequestComplete() const {
    return _requestComplete;
}

bool ClientConnection::isResponseReady() const {
    return _responseReady;
}