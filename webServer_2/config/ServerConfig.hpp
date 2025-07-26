// config/ServerConfig.hpp
#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include "LocationConfig.hpp"
#include <map>


class ServerConfig {
private:
    std::string _host;
    int _port;
    std::string _root;
    std::string _index;

    std::map<std::string, LocationConfig> _locations;
public:
    ServerConfig();
    ~ServerConfig();

    void setHost(const std::string& host);
    void setPort(int port);
    void setRoot(const std::string& root);
    void setIndex(const std::string& index);

    std::string getHost() const;
    int getPort() const;
    std::string getRoot() const;
    std::string getIndex() const;

    void addLocation(const LocationConfig& location);
    const std::map<std::string, LocationConfig>& getLocations() const;
};

#endif
