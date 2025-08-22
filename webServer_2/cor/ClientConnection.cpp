#include "ClientConnection.hpp"
#include <unistd.h>
#include "../Http/HttpRequest.hpp"
#include "../Http/HttpResponse.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>

ClientConnection::ClientConnection(int fd, const ServerConfig& config) 
    : _fd(fd), _requestComplete(false), _responseReady(false), _serverConfig(&config) {}

ClientConnection::~ClientConnection() {
    if (_fd != -1)
        close(_fd);
}

int ClientConnection::getFd() const {
    return _fd;
}

std::string ClientConnection::getContentType(const std::string& filePath) {
    if (filePath.find(".html") != std::string::npos || filePath.find(".htm") != std::string::npos) {
        return "text/html";
    } else if (filePath.find(".css") != std::string::npos) {
        return "text/css";
    } else if (filePath.find(".js") != std::string::npos) {
        return "application/javascript";
    } else if (filePath.find(".json") != std::string::npos) {
        return "application/json";
    } else if (filePath.find(".png") != std::string::npos) {
        return "image/png";
    } else if (filePath.find(".jpg") != std::string::npos || filePath.find(".jpeg") != std::string::npos) {
        return "image/jpeg";
    } else if (filePath.find(".gif") != std::string::npos) {
        return "image/gif";
    } else if (filePath.find(".pdf") != std::string::npos) {
        return "application/pdf";
    } else {
        return "text/plain";
    }
}

std::string ClientConnection::generateDirectoryListing(const std::string& dirPath, const std::string& requestPath) {
    std::ostringstream html;
    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<title>Directory listing for " << requestPath << "</title>\n";
    html << "<style>body{font-family:monospace;margin:40px;} a{text-decoration:none;} a:hover{text-decoration:underline;}</style>\n";
    html << "</head>\n<body>\n";
    html << "<h1>Directory listing for " << requestPath << "</h1>\n<hr>\n<ul>\n";

    DIR* dir = opendir(dirPath.c_str());
    if (dir) {
        struct dirent* entry;
        std::vector<std::string> files;
        std::vector<std::string> dirs;
        
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (name == ".") continue;
            
            std::string fullPath = dirPath + "/" + name;
            struct stat fileStat;
            if (stat(fullPath.c_str(), &fileStat) == 0) {
                if (S_ISDIR(fileStat.st_mode)) {
                    dirs.push_back(name);
                } else {
                    files.push_back(name);
                }
            }
        }
        closedir(dir);
        
        std::sort(dirs.begin(), dirs.end());
        std::sort(files.begin(), files.end());
        
        // Add parent directory link if not root
        if (requestPath != "/") {
            html << "<li><a href=\"../\">📁 ../</a></li>\n";
        }
        
        // Add directories
        for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it) {
            html << "<li><a href=\"" << *it << "/\">📁 " << *it << "/</a></li>\n";
        }
        
        // Add files
        for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
            html << "<li><a href=\"" << *it << "\">📄 " << *it << "</a></li>\n";
        }
    }
    
    html << "</ul>\n<hr>\n<p><em>webserv/1.0</em></p>\n</body>\n</html>\n";
    return html.str();
}

void ClientConnection::handleGet(const LocationConfig* location, const std::string& filePath, HttpResponse& response) {
    struct stat fileStat;
    
    if (stat(filePath.c_str(), &fileStat) != 0) {
        response.setStatusCode(404);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(404, "File not found: " + filePath));
        return;
    }
    
    if (S_ISDIR(fileStat.st_mode)) {
        // Check if autoindex is enabled
        if (location && location->isAutoindex()) {
            response.setStatusCode(200);
            response.setHeader("Content-Type", "text/html");
            response.setBody(generateDirectoryListing(filePath, _request.getUri()));
            return;
        } else {
            response.setStatusCode(403);
            response.setHeader("Content-Type", "text/html");
            response.setBody(generateErrorPage(403, "Directory listing is disabled"));
            return;
        }
    }
    
    // Handle regular file
    std::ifstream file(filePath.c_str(), std::ios::binary);
    if (file.is_open()) {
        std::ostringstream ss;
        ss << file.rdbuf();
        std::string content = ss.str();
        
        response.setStatusCode(200);
        response.setHeader("Content-Type", getContentType(filePath));
        response.setBody(content);
    } else {
        response.setStatusCode(404);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(404, "File not found"));
    }
}

void ClientConnection::handlePost(const LocationConfig* location, const std::string& filePath, HttpResponse& response) {
    (void) filePath;
    if (!location || location->getUploadStore().empty()) {
        response.setStatusCode(400);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(400, "Upload not configured for this location"));
        return;
    }

    const std::string& body = _request.getBody();
    if (body.empty()) {
        response.setStatusCode(400);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(400, "No data to upload"));
        return;
    }

    // Simple file upload - in real implementation you'd parse multipart/form-data
    std::string uploadPath = location->getUploadStore();
    
    // Create upload directory if it doesn't exist
    struct stat st;
    if (stat(uploadPath.c_str(), &st) != 0) {
        if (mkdir(uploadPath.c_str(), 0755) != 0) {
            response.setStatusCode(500);
            response.setHeader("Content-Type", "text/html");
            response.setBody(generateErrorPage(500, "Could not create upload directory"));
            return;
        }
    }

    // Generate unique filename
    time_t rawtime;
    time(&rawtime);
    std::ostringstream filename;
    filename << uploadPath << "/upload_" << rawtime << ".txt";

    std::ofstream outFile(filename.str().c_str(), std::ios::binary);
    if (outFile.is_open()) {
        outFile << body;
        outFile.close();
        
        response.setStatusCode(201);
        response.setHeader("Content-Type", "text/html");
        std::ostringstream successMsg;
        successMsg << "<html><body><h1>Upload Successful</h1>";
        successMsg << "<p>File uploaded to: " << filename.str() << "</p>";
        successMsg << "<p>Size: " << body.length() << " bytes</p>";
        successMsg << "<a href=\"/\">Back to home</a></body></html>";
        response.setBody(successMsg.str());
    } else {
        response.setStatusCode(500);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(500, "Could not save uploaded file"));
    }
}

void ClientConnection::handleDelete(const LocationConfig* location, const std::string& filePath, HttpResponse& response) {
    (void)location; // Suppress unused parameter warning
    
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0) {
        response.setStatusCode(404);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(404, "File not found"));
        return;
    }

    if (S_ISDIR(fileStat.st_mode)) {
        response.setStatusCode(400);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(400, "Cannot delete directories"));
        return;
    }

    if (unlink(filePath.c_str()) == 0) {
        response.setStatusCode(204); // No Content
        response.setHeader("Content-Type", "text/plain");
        response.setBody(""); // Empty body for 204
    } else {
        response.setStatusCode(500);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(500, "Could not delete file"));
    }
}

std::string ClientConnection::generateErrorPage(int statusCode, const std::string& message) {
    std::ostringstream html;
    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<title>Error " << statusCode << "</title>\n";
    html << "<style>body{font-family:Arial,sans-serif;margin:40px;text-align:center;} ";
    html << "h1{color:#d32f2f;} p{color:#666;}</style>\n";
    html << "</head>\n<body>\n";
    html << "<h1>Error " << statusCode << "</h1>\n";
    html << "<p>" << message << "</p>\n";
    html << "<hr><p><em>webserv/1.0</em></p>\n";
    html << "</body>\n</html>\n";
    return html.str();
}

void ClientConnection::generateResponse() {
    std::cout << "[DEBUG] ===== Generating Response =====" << std::endl;
    if (!_request.parse(_readBuffer)) {
        std::cout << "[DEBUG] Failed to parse request -> 400 Bad Request" << std::endl;
        HttpResponse response;
        response.setStatusCode(400);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(400, "Bad Request"));
        _writeBuffer = response.toString();
        _responseReady = true;
        return;
    }

    std::string uri = _request.getUri();
    std::string method = _request.getMethod();

    std::cout << "[DEBUG] Request URI: '" << uri << "'" << std::endl;
    std::cout << "[DEBUG] Request Method: '" << method << "'" << std::endl;

    // Find matching location
    const std::map<std::string, LocationConfig>& locations = _serverConfig->getLocations();
    const LocationConfig* matchedLocation = NULL;
    std::string matchedPath;
    size_t longestMatch = 0;

    for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        const std::string& path = it->first;
        
        if ((uri == path) || (uri.find(path) == 0 && (path == "/" || uri[path.length()] == '/' || uri.length() == path.length()))) {
            if (path.length() > longestMatch) {
                matchedLocation = &it->second;
                matchedPath = path;
                longestMatch = path.length();
            }
        }
    }

    if (!matchedLocation) {
        std::cout << "[DEBUG] No matching location found -> 404 Not Found" << std::endl;
        HttpResponse response;
        response.setStatusCode(404);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(404, "No matching location"));
        _writeBuffer = response.toString();
        _responseReady = true;
        return;
    }

    // Check if method is allowed
    const std::set<std::string>& allowedMethods = matchedLocation->getAllowedMethods();
    if (allowedMethods.find(method) == allowedMethods.end()) {
        std::cout << "[DEBUG] Method '" << method << "' not allowed -> 405 Method Not Allowed" << std::endl;
        HttpResponse response;
        response.setStatusCode(405);
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Allow", getAllowedMethodsString(allowedMethods));
        response.setBody(generateErrorPage(405, "Method Not Allowed"));
        _writeBuffer = response.toString();
        _responseReady = true;
        return;
    }

    // Build file path
    std::string root = matchedLocation->getRoot();
    std::string filePath = buildFilePath(root, matchedPath, uri, matchedLocation);
    
    std::cout << "[DEBUG] Final file path: '" << filePath << "'" << std::endl;

    HttpResponse response;

    // Handle different HTTP methods
    if (method == "GET") {
        handleGet(matchedLocation, filePath, response);
    } else if (method == "POST") {
        handlePost(matchedLocation, filePath, response);
    } else if (method == "DELETE") {
        handleDelete(matchedLocation, filePath, response);
    } else {
        response.setStatusCode(501);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateErrorPage(501, "Method not implemented"));
    }

    _writeBuffer = response.toString();
    _responseReady = true;
    std::cout << "[DEBUG] ===== Response Ready =====" << std::endl;
}

std::string ClientConnection::buildFilePath(const std::string& root, const std::string& matchedPath, 
                                          const std::string& uri, const LocationConfig* location) {
    std::string filePath;
    std::string remainingPath;
    
    if (uri.length() > matchedPath.length()) {
        remainingPath = uri.substr(matchedPath.length());
        if (!remainingPath.empty() && remainingPath[0] == '/' && matchedPath != "/") {
            remainingPath = remainingPath.substr(1);
        }
    }
    
    filePath = root;
    if (!filePath.empty() && filePath[filePath.size() - 1] != '/' && !remainingPath.empty()) {
        filePath += "/";
    }
    
    if (remainingPath.empty()) {
        std::string indexFile = location->getIndex();
        if (indexFile.empty()) {
            indexFile = _serverConfig->getIndex();
        }
        
        if (!filePath.empty() && filePath[filePath.size() - 1] != '/') {
            filePath += "/";
        }
        filePath += indexFile;
    } else {
        filePath += remainingPath;
    }
    
    return filePath;
}

std::string ClientConnection::getAllowedMethodsString(const std::set<std::string>& methods) {
    std::ostringstream oss;
    for (std::set<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
        if (it != methods.begin()) {
            oss << ", ";
        }
        oss << *it;
    }
    return oss.str();
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