#include "config/ConfigParser.hpp"
#include "cor/ServerManager.hpp"
#include <iostream>

int main(int ac, char** av) {
    std::string configPath = "conf/default.conf";

    if (ac == 2)
        configPath = av[1];

    try {
        ConfigParser parser(configPath);
        std::vector<ServerConfig> servers = parser.parse();

        std::cout << "✅ Config loaded successfully!\n";

        for (size_t i = 0; i < servers.size(); ++i) {
            std::cout << "📡 Server " << i + 1 << ": Listening on " 
                      << servers[i].getHost() << ":" << servers[i].getPort() << std::endl;
            std::cout << "    Root: " << servers[i].getRoot() << std::endl;
            std::cout << "    Index: " << servers[i].getIndex() << std::endl;

            const std::map<std::string, LocationConfig>& locations = servers[i].getLocations();
            for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
                const LocationConfig& loc = it->second;
                std::cout << "    📁 Location: " << loc.getPath() << std::endl;
                std::cout << "        Root: " << loc.getRoot() << std::endl;
                std::cout << "        Index: " << loc.getIndex() << std::endl;

                std::cout << "        Methods: ";
                const std::set<std::string>& methods = loc.getAllowedMethods();
                for (std::set<std::string>::const_iterator mit = methods.begin(); mit != methods.end(); ++mit) {
                    std::cout << *mit;
                    std::set<std::string>::const_iterator next_it = mit;
                    ++next_it;
                    if (next_it != methods.end())
                        std::cout << ", ";
                }
                std::cout << std::endl;

                std::cout << "        Upload Store: " << loc.getUploadStore() << std::endl;
                std::cout << "        CGI Extension: " << loc.getCgiExtension() << std::endl;
                std::cout << "        Autoindex: " << (loc.isAutoindex() ? "on" : "off") << std::endl;
            }
        }
        ServerManager manager(servers);
        manager.run();

    } catch (const std::exception& e) {
        std::cerr << "❌ Error loading config: " << e.what() << std::endl;
    }

    return 0;
}
