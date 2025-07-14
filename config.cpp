#include "config.hpp"

Config::Config () {};

Config::~Config() {} ;


void Config::parserConfig(const std::string& path) {
    std::ifstream configFile(path.c_str());
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open config file: " << path << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        if (line.find("server {") != std::string::npos) {
            ServerConfig server = parseServerBlock(configFile);
            _servers.push_back(server);
        }
    }

    configFile.close();
}


ServerConfig Config::parseServerBlock(std::ifstream& file) {
    ServerConfig server;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("}") != std::string::npos) {
        break;
        }
 
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        std::string value;
        iss >> value;

        if (key == "listen") {
            size_t pos = value.find(":");
            server.host = value.substr(0, pos);
            server.port = std::atoi(value.substr(pos + 1).c_str());
        } else if (key == "root") {
            size_t pos = value.find(";");
            server.root = value.substr(0, pos);
        } else if (key == "error_page") {
            std::string l;
            iss >> l;
            size_t pos = l.find(";");
            server.errorPages[std::atoi(value.c_str())] = l.substr(0, pos);
        } else if (key == "location") {
            LocationConfig location = parseLocationBlock(file, value);
            server.locations.push_back(location);
        }
    }

    return server;
}

LocationConfig Config::parseLocationBlock(std::ifstream& file, const std::string& path) {
    LocationConfig location;
    location.path = path;

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("}") != std::string::npos)
            break;

        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "upload_store") {
            std::string val;
            iss >> val;
            size_t pos = val.find(";");
            location.uploadStore = val.substr(0, pos);
        } else if (key == "methods") {
            std::string method;
            while (iss >> method) {
                size_t pos = method.find(";");
                if (pos != std::string::npos)
                    method = method.substr(0, pos);
                location.methods.push_back(method);
            }
        } else if (key == "cgi_pass") {
            std::string cgi;
            iss >> cgi;
            size_t pos = cgi.find(";");
            location.cgiPass = cgi.substr(0, pos);
        }
    }

    return location;
}

const std::vector<ServerConfig>& Config::getServers() const {
    return _servers;
}