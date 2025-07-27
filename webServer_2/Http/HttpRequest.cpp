#include "HttpRequest.hpp"
#include <sstream>
#include <iostream>

HttpRequest::HttpRequest() {}
HttpRequest::~HttpRequest() {}

bool HttpRequest::parse(const std::string& rawRequest) {
    std::istringstream stream(rawRequest);
    std::string line;

    // 🧩 1. First line → Method URI HTTP Version
    if (!std::getline(stream, line) || line.empty())
        return false;

    std::istringstream firstLine(line);
    if (!(firstLine >> _method >> _uri >> _httpVersion))
        return false;

    // 🧩 2. Headers
    while (std::getline(stream, line) && !line.empty() && line != "\r") {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            // remove spaces
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) value.erase(0, 1);
            if (!value.empty() && value[value.size() - 1] == '\r') value.erase(value.size() - 1);
            _headers[key] = value;
        }
    }

    // 🧩 3. Body (if Content-Length exists)
    if (_headers.find("Content-Length") != _headers.end()) {
        std::stringstream bodyStream;
        bodyStream << stream.rdbuf();
        _body = bodyStream.str();
    }

    return true;
}

// 🔍 Getters
const std::string& HttpRequest::getMethod() const { return _method; }
const std::string& HttpRequest::getUri() const { return _uri; }
const std::string& HttpRequest::getHttpVersion() const { return _httpVersion; }
const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return _headers; }
const std::string& HttpRequest::getBody() const { return _body; }
