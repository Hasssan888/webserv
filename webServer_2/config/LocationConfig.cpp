#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : _autoindex(false) {}

LocationConfig::~LocationConfig() {}

void LocationConfig::setPath(const std::string& path) {
    _path = path;
}

void LocationConfig::setRoot(const std::string& root) {
    _root = root;
}

void LocationConfig::setUploadStore(const std::string& store) {
    _uploadStore = store;
}

void LocationConfig::setIndex(const std::string& index) {
    _index = index;
}

void LocationConfig::setCgiExtension(const std::string& ext) {
    _cgiExtension = ext;
}

void LocationConfig::addMethod(const std::string& method) {
    _allowedMethods.insert(method);
}

void LocationConfig::setAutoindex(bool value) {
    _autoindex = value;
}

std::string LocationConfig::getPath() const {
    return _path;
}

std::string LocationConfig::getRoot() const {
    return _root;
}

std::string LocationConfig::getUploadStore() const {
    return _uploadStore;
}

std::string LocationConfig::getIndex() const {
    return _index;
}

std::string LocationConfig::getCgiExtension() const {
    return _cgiExtension;
}

const std::set<std::string>& LocationConfig::getAllowedMethods() const {
    return _allowedMethods;
}

bool LocationConfig::isAutoindex() const {
    return _autoindex;
}
