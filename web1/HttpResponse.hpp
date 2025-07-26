#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HttpResponse {
    private:
        std::string _httpVersion;
        int _statusCode;
        std::string _statusMessage;
        std::map<std::string, std::string> _headers;
        std::string _body;
    
    public:
        HttpResponse();

        void setHttpVersion(const std::string& version);
        void setStatusCode(int code);
        void setStatusMessage(const std::string& message);
        void addHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);
}

#endif