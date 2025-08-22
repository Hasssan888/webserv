#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>
#include <sys/types.h>
#include <signal.h>
#include "../Http/HttpRequest.hpp"
#include "../Http/HttpResponse.hpp"
#include "../config/LocationConfig.hpp"

class CgiHandler {
private:
    void setupCgiEnvironment(const HttpRequest& request, const std::string& scriptPath, const LocationConfig& location);
    std::string getDirectoryPath(const std::string& filePath);
    std::string generateCgiError(const std::string& error);

public:
    CgiHandler();
    ~CgiHandler();
    
    bool isCgiRequest(const std::string& filePath, const LocationConfig& location);
    std::string executeCgi(const std::string& scriptPath, const HttpRequest& request, const LocationConfig& location);
    HttpResponse parseCgiOutput(const std::string& output);
};

#endif