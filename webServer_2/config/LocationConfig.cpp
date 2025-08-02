#include "LocationConfig.hpp"
#include <iostream>

LocationConfig::LocationConfig() : _autoindex(false) , _hasCgiPass(false), _hasCgiExt(false){}

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
    _hasCgiExt = true;
}

void LocationConfig::setCgiPass(const std::string& pass) {
    _cgiPass = pass;
    _hasCgiPass = true;
}

void LocationConfig::addMethod(const std::string& method) {
    _allowedMethods.insert(method);
}

void LocationConfig::setAutoindex(bool value) {
    _autoindex = value;
}

void LocationConfig::setCgiHandlers(std::string key, std::string value) {
    _cgiHandlers[key] = value;
    std::cout << "📝 CGI Handler Added: [" << key << "] => [" << value << "]" << std::endl;
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

std::string LocationConfig::getCgiPass() const {
    return _cgiPass;
}

const std::set<std::string>& LocationConfig::getAllowedMethods() const {
    return _allowedMethods;
}

bool LocationConfig::isAutoindex() const {
    return _autoindex;
}

bool LocationConfig::hasCgiPass() const {
    return _hasCgiPass;
}

bool LocationConfig::hasCgiExt() const {
    return _hasCgiExt;
}

std::map<std::string, std::string> LocationConfig::getCgiHandlers() const {
    return _cgiHandlers;
}
