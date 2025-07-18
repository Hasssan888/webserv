#include "config.hpp"
#include "ServerManager.hpp"


int main(int ac, char **av) {

    if (ac != 2) {
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return 1;
    }

    Config config;
    config.parserConfig(av[1]);

    const std::vector<ServerConfig>& servers = config.getServers();
    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "Server " << i << ":\n";
        std::cout << "  Host: " << servers[i].host << std::endl;
        std::cout << "  Port: " << servers[i].port << std::endl;
        std::cout << "  Root: " << servers[i].root << std::endl;

        for (std::map<int, std::string>::const_iterator it = servers[i].errorPages.begin(); it != servers[i].errorPages.end(); ++it) {
            std::cout << "  Error Page [" << it->first << "]: " << it->second << std::endl;
        }

        for (size_t j = 0; j < servers[i].locations.size(); ++j) {
            std::cout << "  Location: " << servers[i].locations[j].path << std::endl;
            std::cout << "    Upload Store: " << servers[i].locations[j].uploadStore << std::endl;
            std::cout << "    CGI Pass: " << servers[i].locations[j].cgiPass << std::endl;

            std::cout << "    Methods: ";
            for (size_t k = 0; k < servers[i].locations[j].methods.size(); ++k) {
                std::cout << servers[i].locations[j].methods[k] << " ";
            }
            std::cout << std::endl;
        }
    }

    ServerManager manager;
    manager.initServers(servers);

    while (true) {
        std::vector<struct pollfd>& fds = manager.getPollFDs();

        int ready = poll(&fds[0], fds.size(), TIMEOUT);
        if (ready == -1) {
            perror("poll failed");
            break;
        }
        if (ready == 0)
            continue;
        manager.handlePollEvents();
    }

    return 0;
}