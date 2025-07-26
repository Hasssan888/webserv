#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <vector>
#include <string>
#include "ServerConfig_1.hpp"

class ConfigParser {
private:
    std::vector<ServerConfig> _servers;

public:
    void parseFile(const std::string& path);
    const std::vector<ServerConfig>& getServers() const;
};

#endif
