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

        std::string getStatusMessage(int code) const;
    
    public:
        HttpResponse();
        ~HttpResponse();

        void setStatusCode(int code);
        void setHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);

        std::string toString() const;
};



#endif