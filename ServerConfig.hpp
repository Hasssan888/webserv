#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>

#define TIMEOUT 5000


struct LocationConfig {
    std::string path;
    std::string uploadStore;
    std::vector<std::string> methods;
    std::string cgiPass;
};

struct ServerConfig {
    std::string host;
    int port;
    std::string root;
    std::map<int, std::string> errorPages;
    std::vector<LocationConfig> locations;
};


#endif