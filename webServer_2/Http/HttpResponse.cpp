#include "HttpResponse.hpp"
#include <sstream>

HttpResponse::HttpResponse()
    : _httpVersion("HTTP/1.1"), _statusCode(200), _statusMessage("OK") {}

HttpResponse::~HttpResponse() {}

void    HttpResponse::setStatusCode(int code) {
    _statusCode = code;
    _statusMessage = getStatusMessage(code);
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string& body) {
    _body = body;
    std::stringstream ss;
    ss << _body.size();
    _headers["Content-Length"] = ss.str();
}

std::string HttpResponse::getStatusMessage(int code) const {
    switch (code)
    {
    case  200:
        return "OK";
    case  404:
        return "Not Found";
    case  500:
        return "Internal Server Error";
    
    default:
        return "Unknown Status";
    }
}

std::string HttpResponse::toString() const {
    std::ostringstream response;

    response << _httpVersion << " " << _statusCode << " " << _statusMessage << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
            it != _headers.end(); ++it) {
                response << it->first << ": " << it->second << "\r\n";
            }
    response << "\r\n";
    response << _body;

    return response.str();
}