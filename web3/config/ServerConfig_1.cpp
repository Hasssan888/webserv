#include "ServerConfig_1.hpp"

ServerConfig::ServerConfig()
    : _port(80), _autoindex(false) {}

void ServerConfig::setPort(int port) { _port = port; }

void ServerConfig::setHost(const std::string& host) { _host = host; }

void ServerConfig::setServerName(const std::string& name) { _serverName = name; }

void ServerConfig::setRoot(const std::string& root) { _root = root; }

void ServerConfig::setAutoindex(bool value) { _autoindex = value; }

void ServerConfig::setErrorPage(int code, const std::string& uri) {
    _errorPages[code] = uri;
}

void ServerConfig::addLocation(const LocationConfig& loc) {
    _locations[loc.path] = loc;
}

int ServerConfig::getPort() const { return _port; }

const std::string& ServerConfig::getHost() const { return _host; }

const std::string& ServerConfig::getServerName() const { return _serverName; }

const std::string& ServerConfig::getRoot() const { return _root; }

bool ServerConfig::getAutoindex() const { return _autoindex; }

const std::map<int, std::string>& ServerConfig::getErrorPages() const {
    return _errorPages;
}

const std::map<std::string, LocationConfig>& ServerConfig::getLocations() const {
    return _locations;
}
