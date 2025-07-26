// config/LocationConfig.hpp
#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <set>

class LocationConfig {
private:
    std::string _path;
    std::string _root;
    std::string _uploadStore;
    std::string _index;
    std::string _cgiExtension;
    std::set<std::string> _allowedMethods;
    bool _autoindex;

public:
    LocationConfig();
    ~LocationConfig();

    void setPath(const std::string& path);
    void setRoot(const std::string& root);
    void setUploadStore(const std::string& store);
    void setIndex(const std::string& index);
    void setCgiExtension(const std::string& ext);
    void addMethod(const std::string& method);
    void setAutoindex(bool value);

    std::string getPath() const;
    std::string getRoot() const;
    std::string getUploadStore() const;
    std::string getIndex() const;
    std::string getCgiExtension() const;
    const std::set<std::string>& getAllowedMethods() const;
    bool isAutoindex() const;
};

#endif
