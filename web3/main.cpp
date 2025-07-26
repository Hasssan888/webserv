#include "core/ServerManager_1.hpp"
#include "config/ConfigParser.hpp"
#include <iostream>
#include <stdexcept>

int main(int ac, char** av) {
    try {
        std::string configPath = "default.conf";
        if (ac == 2)
            configPath = av[1];
        
        ConfigParser parser;
        parser.parseFile(configPath);
        
        std::vector<ServerConfig> configs = parser.getServers();

        ServerManager manger(configs);
        manger.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}