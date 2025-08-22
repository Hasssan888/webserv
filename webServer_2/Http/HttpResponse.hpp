#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <ctime>

class HttpResponse {
private:
    std::string _httpVersion;
    int _statusCode;
    std::string _statusMessage;
    std::map<std::string, std::string> _headers;
    std::string _body;

    std::string getStatusMessage(int code) const;

public:
    HttpResponse();
    ~HttpResponse();

    void setStatusCode(int code);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);
    
    // Utility methods
    void setRedirect(const std::string& location, int code = 302);
    void setCacheControl(const std::string& directive);
    void setContentEncoding(const std::string& encoding);

    std::string toString() const;
};

#endif