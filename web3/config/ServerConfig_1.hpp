#ifndef SERVERCONFIG_1_HPP
#define SERVERCONFIG_1_HPP

#include <string>
#include <map>
#include <vector>

struct LocationConfig {
    std::string path;
    std::string root;
    bool autoindex;
    std::string uploadStore;
    std::string cgiPass;
    std::vector<std::string> methods;

    LocationConfig() : autoindex(false) {}
};

class ServerConfig {
private:
    int _port;
    std::string _host;
    std::string _serverName;
    std::string _root;
    bool _autoindex;
    std::map<int, std::string> _errorPages;
    std::map<std::string, LocationConfig> _locations;

public:
    ServerConfig();

    void setPort(int port);
    void setHost(const std::string& host);            // ✅ NEW
    void setServerName(const std::string& name);
    void setRoot(const std::string& root);
    void setAutoindex(bool value);
    void setErrorPage(int code, const std::string& uri); // ✅ NEW
    void addLocation(const LocationConfig& loc);

    int getPort() const;
    const std::string& getHost() const;              // ✅ NEW
    const std::string& getServerName() const;
    const std::string& getRoot() const;
    bool getAutoindex() const;
    const std::map<int, std::string>& getErrorPages() const; // ✅ NEW
    const std::map<std::string, LocationConfig>& getLocations() const;
};

#endif
