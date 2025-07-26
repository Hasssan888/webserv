#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
    : _host("127.0.0.1"), _port(8080), _root("/var/www/html"), _index("index.html") {}

ServerConfig::~ServerConfig() {}

void ServerConfig::setHost(const std::string& host) {
    _host = host;
}

void ServerConfig::setPort(int port) {
    _port = port;
}

void ServerConfig::setRoot(const std::string& root) {
    _root = root;
}

void ServerConfig::setIndex(const std::string& index) {
    _index = index;
}

std::string ServerConfig::getHost() const {
    return _host;
}

int ServerConfig::getPort() const {
    return _port;
}

std::string ServerConfig::getRoot() const {
    return _root;
}

std::string ServerConfig::getIndex() const {
    return _index;
}

void ServerConfig::addLocation(const LocationConfig& location) {
    _locations[location.getPath()] = location;
}

const std::map<std::string, LocationConfig>& ServerConfig::getLocations() const {
    return _locations;
}
