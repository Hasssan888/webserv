#include "HttpRequest.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>

HttpRequest::HttpRequest() : _contentLength(0) {}
HttpRequest::~HttpRequest() {}

std::string HttpRequest::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string HttpRequest::toLowerCase(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool HttpRequest::parse(const std::string& rawRequest) {
    std::istringstream stream(rawRequest);
    std::string line;

    // Parse request line (METHOD URI HTTP/VERSION)
    if (!std::getline(stream, line) || line.empty())
        return false;

    // Remove trailing \r if present
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);

    std::istringstream firstLine(line);
    if (!(firstLine >> _method >> _uri >> _httpVersion))
        return false;

    // Validate HTTP version
    if (_httpVersion != "HTTP/1.0" && _httpVersion != "HTTP/1.1") {
        std::cerr << "[WARNING] Unsupported HTTP version: " << _httpVersion << std::endl;
        return false;
    }

    // Parse query string from URI
    size_t queryPos = _uri.find('?');
    if (queryPos != std::string::npos) {
        _queryString = _uri.substr(queryPos + 1);
        _uri = _uri.substr(0, queryPos);
    }

    // Parse headers
    while (std::getline(stream, line) && !line.empty() && line != "\r") {
        // Remove trailing \r if present
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Convert header names to lowercase for case-insensitive comparison
            _headers[toLowerCase(key)] = value;
            _originalHeaders[key] = value; // Keep original case for response
        }
    }

    // Parse Content-Length if present
    std::map<std::string, std::string>::const_iterator it = _headers.find("content-length");
    if (it != _headers.end()) {
        _contentLength = std::atoi(it->second.c_str());
        
        // Read body based on Content-Length
        if (_contentLength > 0) {
            std::stringstream bodyStream;
            bodyStream << stream.rdbuf();
            _body = bodyStream.str();
            
            // Ensure we have the complete body
            if (static_cast<int>(_body.length()) < _contentLength) {
                // Body is incomplete, but we'll accept what we have for now
                std::cerr << "[WARNING] Body incomplete. Expected: " << _contentLength 
                         << ", Got: " << _body.length() << std::endl;
            } else if (static_cast<int>(_body.length()) > _contentLength) {
                // Trim body to Content-Length
                _body = _body.substr(0, _contentLength);
            }
        }
    }

    // Parse Transfer-Encoding for chunked requests
    it = _headers.find("transfer-encoding");
    if (it != _headers.end() && toLowerCase(it->second) == "chunked") {
        return parseChunkedBody(stream);
    }

    return true;
}

bool HttpRequest::parseChunkedBody(std::istringstream& stream) {
    std::string line;
    _body.clear();
    
    while (std::getline(stream, line)) {
        // Remove trailing \r if present
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        
        // Parse chunk size (hexadecimal)
        std::istringstream hexStream(line);
        int chunkSize;
        if (!(hexStream >> std::hex >> chunkSize)) {
            std::cerr << "[ERROR] Invalid chunk size: " << line << std::endl;
            return false;
        }
        
        // If chunk size is 0, we've reached the end
        if (chunkSize == 0) {
            // Skip trailer headers (if any) and final CRLF
            while (std::getline(stream, line) && !line.empty() && line != "\r") {
                // Ignore trailer headers for now
            }
            break;
        }
        
        // Read chunk data
        std::string chunk;
        chunk.resize(chunkSize);
        if (stream.read(&chunk[0], chunkSize)) {
            _body += chunk;
        } else {
            std::cerr << "[ERROR] Failed to read chunk data" << std::endl;
            return false;
        }
        
        // Skip trailing CRLF after chunk data
        std::getline(stream, line);
    }
    
    return true;
}

// Getters
const std::string& HttpRequest::getMethod() const { return _method; }
const std::string& HttpRequest::getUri() const { return _uri; }
const std::string& HttpRequest::getHttpVersion() const { return _httpVersion; }
const std::string& HttpRequest::getBody() const { return _body; }
const std::string& HttpRequest::getQueryString() const { return _queryString; }
int HttpRequest::getContentLength() const { return _contentLength; }

const std::map<std::string, std::string>& HttpRequest::getHeaders() const { 
    return _originalHeaders; 
}

std::string HttpRequest::getHeader(const std::string& name) const {
    std::string lowerName = toLowerCase(name);
    std::map<std::string, std::string>::const_iterator it = _headers.find(lowerName);
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

bool HttpRequest::hasHeader(const std::string& name) const {
    std::string lowerName = toLowerCase(name);
    return _headers.find(lowerName) != _headers.end();
}

std::map<std::string, std::string> HttpRequest::parseQueryString() const {
    std::map<std::string, std::string> params;
    if (_queryString.empty()) return params;
    
    std::istringstream queryStream(_queryString);
    std::string pair;
    
    while (std::getline(queryStream, pair, '&')) {
        size_t pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = urlDecode(pair.substr(0, pos));
            std::string value = urlDecode(pair.substr(pos + 1));
            params[key] = value;
        } else {
            params[urlDecode(pair)] = "";
        }
    }
    
    return params;
}

std::string HttpRequest::urlDecode(const std::string& str) const {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            char* endptr;
            long value = std::strtol(hex.c_str(), &endptr, 16);
            if (*endptr == '\0') {
                result += static_cast<char>(value);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    
    return result;
}

bool HttpRequest::isKeepAlive() const {
    std::string connection = getHeader("connection");
    if (!connection.empty()) {
        return toLowerCase(connection) == "keep-alive";
    }
    
    // HTTP/1.1 defaults to keep-alive, HTTP/1.0 defaults to close
    return _httpVersion == "HTTP/1.1";
}

std::string HttpRequest::getContentType() const {
    return getHeader("content-type");
}

std::string HttpRequest::getUserAgent() const {
    return getHeader("user-agent");
}

std::string HttpRequest::getHost() const {
    return getHeader("host");
}