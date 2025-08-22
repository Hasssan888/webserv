#include "CgiHandler.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>

CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}

bool CgiHandler::isCgiRequest(const std::string& filePath, const LocationConfig& location) {
    std::string extension = location.getCgiExtension();
    if (extension.empty()) return false;
    
    // Check if file has the CGI extension
    size_t pos = filePath.rfind('.');
    if (pos == std::string::npos) return false;
    
    std::string fileExt = filePath.substr(pos);
    return fileExt == extension;
}

std::string CgiHandler::executeCgi(const std::string& scriptPath, const HttpRequest& request, const LocationConfig& location) {
    int pipeIn[2], pipeOut[2];
    
    // Create pipes for communication with CGI process
    if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1) {
        std::cerr << "[CGI ERROR] Failed to create pipes" << std::endl;
        return generateCgiError("Failed to create pipes");
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "[CGI ERROR] Failed to fork process" << std::endl;
        close(pipeIn[0]); close(pipeIn[1]);
        close(pipeOut[0]); close(pipeOut[1]);
        return generateCgiError("Failed to fork process");
    }
    
    if (pid == 0) {
        // Child process
        
        // Close unused pipe ends
        close(pipeIn[1]);  // Close write end of input pipe
        close(pipeOut[0]); // Close read end of output pipe
        
        // Redirect stdin and stdout
        dup2(pipeIn[0], STDIN_FILENO);
        dup2(pipeOut[1], STDOUT_FILENO);
        
        // Close original pipe file descriptors
        close(pipeIn[0]);
        close(pipeOut[1]);
        
        // Set up environment variables
        setupCgiEnvironment(request, scriptPath, location);
        
        // Change to script directory
        std::string scriptDir = getDirectoryPath(scriptPath);
        if (!scriptDir.empty() && chdir(scriptDir.c_str()) != 0) {
            std::cerr << "[CGI ERROR] Failed to change directory to: " << scriptDir << std::endl;
        }
        
        // Determine interpreter based on extension
        std::string extension = location.getCgiExtension();
        const char* interpreter = NULL;
        const char* scriptFile = scriptPath.c_str();
        
        if (extension == ".php") {
            interpreter = "php-cgi";
        } else if (extension == ".py") {
            interpreter = "python3";
        } else if (extension == ".pl") {
            interpreter = "perl";
        }
        
        if (interpreter) {
            execl("/usr/bin/env", "env", interpreter, scriptFile, (char*)NULL);
        } else {
            // Try to execute directly
            execl(scriptFile, scriptFile, (char*)NULL);
        }
        
        // If we reach here, exec failed
        std::cerr << "[CGI ERROR] Failed to execute CGI script: " << scriptPath << std::endl;
        _exit(1);
    } else {
        // Parent process
        
        // Close unused pipe ends
        close(pipeIn[0]);  // Close read end of input pipe
        close(pipeOut[1]); // Close write end of output pipe
        
        // Send request body to CGI process
        const std::string& body = request.getBody();
        if (!body.empty()) {
            ssize_t written = write(pipeIn[1], body.c_str(), body.length());
            if (written == -1) {
                std::cerr << "[CGI ERROR] Failed to write to CGI process" << std::endl;
            }
        }
        close(pipeIn[1]); // Close write end to signal EOF
        
        // Read CGI output
        std::string output;
        char buffer[1024];
        ssize_t bytesRead;
        
        // Set read timeout
        fd_set readSet;
        struct timeval timeout;
        timeout.tv_sec = 30; // 30 second timeout
        timeout.tv_usec = 0;
        
        while (true) {
            FD_ZERO(&readSet);
            FD_SET(pipeOut[0], &readSet);
            
            int result = select(pipeOut[0] + 1, &readSet, NULL, NULL, &timeout);
            if (result == -1) {
                std::cerr << "[CGI ERROR] Select failed" << std::endl;
                break;
            } else if (result == 0) {
                std::cerr << "[CGI ERROR] CGI script timeout" << std::endl;
                kill(pid, SIGKILL);
                break;
            }
            
            bytesRead = read(pipeOut[0], buffer, sizeof(buffer) - 1);
            if (bytesRead <= 0) break;
            
            buffer[bytesRead] = '\0';
            output += buffer;
        }
        
        close(pipeOut[0]);
        
        // Wait for child process to finish
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return output;
        } else {
            std::cerr << "[CGI ERROR] CGI script exited with error status" << std::endl;
            return generateCgiError("CGI script execution failed");
        }
    }
    
    return generateCgiError("Unknown error");
}

void CgiHandler::setupCgiEnvironment(const HttpRequest& request, const std::string& scriptPath, const LocationConfig& location) {
    // Required CGI environment variables
    setenv("REQUEST_METHOD", request.getMethod().c_str(), 1);
    setenv("REQUEST_URI", request.getUri().c_str(), 1);
    setenv("SCRIPT_NAME", scriptPath.c_str(), 1);
    setenv("SCRIPT_FILENAME", scriptPath.c_str(), 1);
    
    // Query string
    if (!request.getQueryString().empty()) {
        setenv("QUERY_STRING", request.getQueryString().c_str(), 1);
    }
    
    // Content info
    if (request.getContentLength() > 0) {
        std::ostringstream oss;
        oss << request.getContentLength();
        setenv("CONTENT_LENGTH", oss.str().c_str(), 1);
    }
    
    std::string contentType = request.getContentType();
    if (!contentType.empty()) {
        setenv("CONTENT_TYPE", contentType.c_str(), 1);
    }
    
    // Server info
    setenv("SERVER_SOFTWARE", "webserv/1.0", 1);
    setenv("SERVER_NAME", "localhost", 1);
    setenv("SERVER_PORT", "8080", 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("SERVER_PROTOCOL", request.getHttpVersion().c_str(), 1);
    
    // Path info
    setenv("PATH_INFO", "", 1);
    setenv("PATH_TRANSLATED", "", 1);
    
    // Remote info (simplified)
    setenv("REMOTE_ADDR", "127.0.0.1", 1);
    setenv("REMOTE_HOST", "", 1);
    setenv("REMOTE_USER", "", 1);
    
    // HTTP headers as environment variables
    const std::map<std::string, std::string>& headers = request.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::string envName = "HTTP_" + it->first;
        
        // Convert to uppercase and replace - with _
        for (size_t i = 0; i < envName.length(); ++i) {
            if (envName[i] == '-') {
                envName[i] = '_';
            } else {
                envName[i] = std::toupper(envName[i]);
            }
        }
        
        setenv(envName.c_str(), it->second.c_str(), 1);
    }
    
    (void)location; // Suppress unused parameter warning
}

std::string CgiHandler::getDirectoryPath(const std::string& filePath) {
    size_t pos = filePath.rfind('/');
    if (pos == std::string::npos) return "";
    return filePath.substr(0, pos);
}

std::string CgiHandler::generateCgiError(const std::string& error) {
    std::ostringstream response;
    response << "Status: 500 Internal Server Error\r\n";
    response << "Content-Type: text/html\r\n";
    response << "\r\n";
    response << "<!DOCTYPE html>\n<html>\n<head>\n<title>CGI Error</title>\n</head>\n<body>\n";
    response << "<h1>CGI Error</h1>\n<p>" << error << "</p>\n";
    response << "</body>\n</html>\n";
    return response.str();
}

HttpResponse CgiHandler::parseCgiOutput(const std::string& output) {
    HttpResponse response;
    
    if (output.empty()) {
        response.setStatusCode(500);
        response.setHeader("Content-Type", "text/html");
        response.setBody(generateCgiError("Empty CGI output"));
        return response;
    }
    
    // Find the end of headers (double CRLF)
    size_t headerEnd = output.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        headerEnd = output.find("\n\n");
        if (headerEnd == std::string::npos) {
            // No headers found, treat entire output as body
            response.setStatusCode(200);
            response.setHeader("Content-Type", "text/html");
            response.setBody(output);
            return response;
        }
    }
    
    std::string headers = output.substr(0, headerEnd);
    std::string body = output.substr(headerEnd + (output[headerEnd + 2] == '\r' ? 4 : 2));
    
    // Parse CGI headers
    std::istringstream headerStream(headers);
    std::string line;
    bool statusSet = false;
    
    response.setStatusCode(200); // Default status
    
    while (std::getline(headerStream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string headerName = line.substr(0, colonPos);
            std::string headerValue = line.substr(colonPos + 1);
            
            // Trim whitespace
            while (!headerValue.empty() && (headerValue[0] == ' ' || headerValue[0] == '\t')) {
                headerValue.erase(0, 1);
            }
            
            if (headerName == "Status") {
                // Parse status code
                std::istringstream statusStream(headerValue);
                int statusCode;
                if (statusStream >> statusCode) {
                    response.setStatusCode(statusCode);
                    statusSet = true;
                }
            } else {
                response.setHeader(headerName, headerValue);
            }
        }
    }
    
    response.setBody(body);
    return response;
}