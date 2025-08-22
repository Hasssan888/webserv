#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include "../Http/HttpRequest.hpp"
#include "../Http/HttpResponse.hpp"
#include "../config/ServerConfig.hpp"

// Forward declaration to avoid circular dependency
class CgiHandler;

class ClientConnection {
private:
    int _fd;
    std::string _readBuffer;
    std::string _writeBuffer;
    bool _requestComplete;
    bool _responseReady;
    HttpRequest _request;
    const ServerConfig* _serverConfig;

    // Helper methods
    std::string getContentType(const std::string& filePath);
    std::string generateDirectoryListing(const std::string& dirPath, const std::string& requestPath);
    std::string generateErrorPage(int statusCode, const std::string& message);
    std::string buildFilePath(const std::string& root, const std::string& matchedPath, 
                            const std::string& uri, const LocationConfig* location);
    std::string getAllowedMethodsString(const std::set<std::string>& methods);

    // HTTP method handlers
    void handleGet(const LocationConfig* location, const std::string& filePath, HttpResponse& response);
    void handlePost(const LocationConfig* location, const std::string& filePath, HttpResponse& response);
    void handleDelete(const LocationConfig* location, const std::string& filePath, HttpResponse& response);

public:
    ClientConnection(int fd, const ServerConfig& config);
    ~ClientConnection();

    int getFd() const;
    bool isRequestComplete() const;
    bool isResponseReady() const;
    bool isDone() const;

    void readRequest();
    void generateResponse();
    void sendResponse();
};

#endif