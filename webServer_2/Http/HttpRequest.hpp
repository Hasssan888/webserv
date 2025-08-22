#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <sstream>

class HttpRequest {
private:
    std::string _method;
    std::string _uri;
    std::string _httpVersion;
    std::string _queryString;
    std::map<std::string, std::string> _headers; // lowercase keys
    std::map<std::string, std::string> _originalHeaders; // original case keys
    std::string _body;
    int _contentLength;
    
    // Helper methods
    std::string trim(const std::string& str);
    std::string toLowerCase(const std::string& str) const;
    bool parseChunkedBody(std::istringstream& stream);
    std::string urlDecode(const std::string& str) const;

public:
    HttpRequest();
    ~HttpRequest();

    bool parse(const std::string& rawRequest);

    // Getters
    const std::string& getMethod() const;
    const std::string& getUri() const;
    const std::string& getHttpVersion() const;
    const std::string& getBody() const;
    const std::string& getQueryString() const;
    int getContentLength() const;
    const std::map<std::string, std::string>& getHeaders() const;
    
    // Header utilities
    std::string getHeader(const std::string& name) const;
    bool hasHeader(const std::string& name) const;
    
    // Common headers
    std::string getContentType() const;
    std::string getUserAgent() const;
    std::string getHost() const;
    
    // Utility methods
    bool isKeepAlive() const;
    std::map<std::string, std::string> parseQueryString() const;
};

#endif