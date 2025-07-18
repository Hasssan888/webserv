#include "HttpRequest.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>


HttpRequest::HttpRequest() {}

void HttpRequest::setMethod(const std::string& method) {
    _method = method;
}

void HttpRequest::setUri(const std::string& uri) {
    _uri = uri;
}

void HttpRequest::setHttpVersion(const std::string& version) {
    _httpVersion = version;
}

void HttpRequest::addHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

void HttpRequest::setBody(const std::string& body) {
    _body = body;
}

const std::string& HttpRequest::getMethod() const {
    return _method;
}

const std::string& HttpRequest::getUri() const {
    return _uri;
}

const std::string& HttpRequest::getHttpVersion() const {
    return _httpVersion;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
    return _headers;
}

const std::string& HttpRequest::getBody() const {
    return _body;
}

bool parseHttpRequest(const std::string& rawRequest, Httprequest& request) {
    std::istringstream stream(rawRequest);
    std::string line;

    // ===== 1. Parse Request Line =====
    if (!std::getline(stream, line))
        return false;

    std::istringstream requestLine(line);
    std::string method, uri, version;
    requestLine >> method >> uri >> version;

    if (method.empty() || uri.empty() || version.empty())
        return false;
    
    request.setMethod(method);
    request.setUri(uri);
    request.setHttpVersion(version);

    // ===== 2. Parse Headers =====
    while (std::getline(stream, line) && line != "\r") {
        size_t colon = line.find(':');
        if (colon == std::string::npos)
            continue;
        
        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);

        // Trim spaces
        key.erase(key.find_last_not_of(" \r") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_first_not_of("\r") + 1);

        request.addHeader(key, value);
    }

    // ===== 3. Parse Body (if any) =====
    std::string body;
    std::getline(stream, body,'\0');
    request.setBody(body);

    return true;
}

bool HttpRequest::isComplete() const {
    // إذا ماكايناش نهاية الهيدرز "\r\n\r\n"، راه request مازال ناقص
    if (_headers.empty())
        return false;

    // شي request ما كيحتاجش body (بحال GET)، نعتبروه complete من بعد headers
    if (_method == "GET" || _method == "DELETE")
        return true;

    // POST ولا غيرو كيتطلب Content-Length
    std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Length");
    if (it != _headers.end()) {
        int contentLength = std::atoi(it->second.c_str());
        return (int)_body.size() >= contentLength;
    }

    // مكاينش Content-Length → ما يمكنش نعرف واش complete
    return false;
}
