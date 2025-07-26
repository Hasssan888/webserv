// config/ConfigParser.hpp
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include "ServerConfig.hpp"

class ConfigParser {
private:
    std::string _filePath;

public:
    ConfigParser(const std::string& path);
    ~ConfigParser();

    std::vector<ServerConfig> parse();
};

#endif
