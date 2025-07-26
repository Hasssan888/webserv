#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos)
        return "";
    return str.substr(start, end - start + 1);
}

void ConfigParser::parseFile(const std::string& filename) {
    std::cout << "Parsing config file: " << filename << std::endl;
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file: " << filename << std::endl;
        return; // <-- التصحيح هنا
    }


    std::string line;
    ServerConfig currentServer;
    LocationConfig currentLocation;
    bool isInServerBlock = false;
    bool isInLocationBlock = false;

    while (std::getline(file, line)) {
        std::cout << "[DEBUG] line read: " << line << std::endl;
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        // Start server block
        if (line == "server {") {
            currentServer = ServerConfig();
            isInServerBlock = true;
            continue;
        }

        // Start location block
        if (line.find("location") == 0 && line.find("{") != std::string::npos) {
            std::istringstream iss(line);
            std::string directive, path, brace;
            iss >> directive >> path >> brace;
            currentLocation = LocationConfig();
            currentLocation.path = trim(path);
            isInLocationBlock = true;
            continue;
        }

        // End location block
        if (line == "}" && isInLocationBlock) {
            currentServer.addLocation(currentLocation);
            isInLocationBlock = false;
            continue;
        }

        // End server block
        if (line == "}" && isInServerBlock) {
            _servers.push_back(currentServer);
            isInServerBlock = false;
            continue;
        }

        // Parse directives
        std::istringstream iss(line);
        std::string directive;
        iss >> directive;

        if (directive == "listen") {
            std::string value;
            iss >> value;
            size_t colonPos = value.find(':');
            if (colonPos != std::string::npos) {
                std::string ip = value.substr(0, colonPos);
                int port = std::atoi(value.substr(colonPos + 1).c_str());
                currentServer.setHost(ip);
                currentServer.setPort(port);
            } else {
                currentServer.setPort(std::atoi(value.c_str()));
            }
        } else if (directive == "server_name" || directive == "name") {
            std::string name;
            iss >> name;
            currentServer.setServerName(trim(name));
        } else if (directive == "root") {
            std::string root;
            iss >> root;
            if (isInLocationBlock)
                currentLocation.root = trim(root);
            else
                currentServer.setRoot(trim(root));
        } else if (directive == "autoindex") {
            std::string value;
            iss >> value;
            bool on = (value == "on");
            if (isInLocationBlock)
                currentLocation.autoindex = on;
            else
                currentServer.setAutoindex(on);
        } else if (directive == "error_page") {
            int code;
            std::string uri;
            iss >> code >> uri;
            currentServer.setErrorPage(code, uri);
        } else if (directive == "upload_store") {
            std::string path;
            iss >> path;
            currentLocation.uploadStore = trim(path);
        } else if (directive == "methods") {
            std::string method;
            while (iss >> method)
                currentLocation.methods.push_back(method);
        } else if (directive == "cgi_pass") {
            std::string path;
            iss >> path;
            currentLocation.cgiPass = trim(path);
        }
    }

    file.close();
}

const std::vector<ServerConfig>& ConfigParser::getServers() const {
    return _servers;
}
