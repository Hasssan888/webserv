#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class Httprequest {
    private:
        std::string _method;
        std::string _uri;
        std::string _httpVersion;
        std::map<std::string, std::string> _hearders;
        std::string _body;
    public:
        Httprequest();

        void setMethod(const std::string& method);
        void setUri(const std::string& uri);
        void setHttpVersion(const std::string& version);
        void addHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);

        const std::string& getMethod() const;
        const std::string& getUri() const;
        const std::string& getHttVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getBody() const;

        bool isComplete() const;
};



#endif