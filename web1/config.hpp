#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"

class Config {
    private:
        std::vector<ServerConfig> _servers;
        ServerConfig parseServerBlock(std::ifstream& file);
        LocationConfig parseLocationBlock(std::ifstream& file, const std::string& path);
    public:
        Config();
        ~Config();

        void parserConfig(const std::string& path);

        const std::vector<ServerConfig>& getServers() const;
};

#endif