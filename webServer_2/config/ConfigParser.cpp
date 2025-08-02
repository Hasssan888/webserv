#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cstdlib>


ConfigParser::ConfigParser(const std::string& path) : _filePath(path) {}

ConfigParser::~ConfigParser() {}


static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string trimSemicolon(const std::string& str) {
    std::string s = str;
    if (!s.empty() && s[s.length() - 1] == ';')
        s = s.substr(0, s.length() - 1);
    return s;
}

std::vector<ServerConfig> ConfigParser::parse() {
    std::ifstream file(_filePath.c_str());
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file");

    std::string line;
    bool inServerBlock = false;
    ServerConfig currentServer;
    std::vector<ServerConfig> servers;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        if (line == "server {") {
            if (inServerBlock)
                throw std::runtime_error("Nested server blocks not allowed");
            inServerBlock = true;
            currentServer = ServerConfig();
        } else if (line == "}") {
            if (!inServerBlock)
                throw std::runtime_error("Unexpected closing bracket");
            inServerBlock = false;
            servers.push_back(currentServer);
        } else if (inServerBlock) {
            std::istringstream iss(line);
            std::string key;
            iss >> key;

            if (key == "listen") {
                std::string addr;
                iss >> addr;
                addr = trimSemicolon(addr);
                size_t colon = addr.find(':');
                if (colon == std::string::npos)
                    throw std::runtime_error("Invalid listen format (expected host:port)");

                std::string host = addr.substr(0, colon);
                int port = std::atoi(addr.substr(colon + 1).c_str());
                currentServer.setHost(host);
                currentServer.setPort(port);
            } else if (key == "root") {
                std::string value;
                iss >> value;
                value = trimSemicolon(value);
                currentServer.setRoot(value);
            } else if (key == "index") {
                std::string value;
                iss >> value;
                value = trimSemicolon(value);
                currentServer.setIndex(value);
            } 
            else if (key == "location") {
                std::string path;
                iss >> path;

                std::string rest;
                std::getline(iss, rest);
                rest = trim(rest);

                // كيتحقق واش الباقي فيه { ولا لا
                bool hasOpeningBrace = (rest == "{");

                if (!hasOpeningBrace) {
                    if (!std::getline(file, line))
                        throw std::runtime_error("Unexpected EOF, expected '{' after location path");

                    line = trim(line);
                    if (line != "{")
                        throw std::runtime_error("Expected '{' after location path");
                }


                LocationConfig loc;
                loc.setPath(path);

                // قراءة داخل بلوك location
                while (std::getline(file, line)) {
                    line = trim(line);
                    if (line == "}")
                        break;

                    std::istringstream iss2(line);
                    std::string subkey;
                    iss2 >> subkey;

                    if (subkey == "methods") {
                        std::string method;
                        while (iss2 >> method) {
                            method = trimSemicolon(method);  // ✅ Remove semicolon from methods
                            if (!method.empty()) {
                                loc.addMethod(method);
                            }
                        }
                    } else if (subkey == "root") {
                        std::string value;
                        iss2 >> value;
                        value = trimSemicolon(value);
                        loc.setRoot(value);
                    } else if (subkey == "upload_store") {
                        std::string value;
                        iss2 >> value;
                        value = trimSemicolon(value);
                        loc.setUploadStore(value);
                    } else if (subkey == "index") {
                        std::string value;
                        iss2 >> value;
                        value = trimSemicolon(value);
                        loc.setIndex(value);
                    } else if (subkey == "cgi_extension") {
                        
                        std::string value;
                        iss2 >> value;
                        std::cout << "************ cgi_extension: " << value << " *********"<< std::endl;
                        value = trimSemicolon(value);
                        loc.setCgiExtension(value);
                    } else if (subkey == "cgi_pass"){
                        
                        std::string value;
                        iss2 >> value;
                        std::cout << "************ cgi_pass: " << value << " *********"<< std::endl;
                        value = trimSemicolon(value);
                        loc.setCgiPass(value);
                    } else if (subkey == "autoindex") {
                        std::string value;
                        iss2 >> value;
                        value = trimSemicolon(value);
                        loc.setAutoindex(value == "on");
                    } else {
                        std::cerr << "⚠️ Unknown location directive: " << subkey << std::endl;
                    }
                
                }
                if (loc.hasCgiExt() && loc.hasCgiPass()) {
                    loc.setCgiHandlers(loc.getCgiExtension(), loc.getCgiPass());
                }
                    std::cout << "📂 CGI Handlers for location " << loc.getPath() << ":\n";
                    const std::map<std::string, std::string>& cgiMap = loc.getCgiHandlers();
                    for (std::map<std::string, std::string>::const_iterator it = cgiMap.begin(); it != cgiMap.end(); ++it) {
                        std::cout << "   [ " << it->first << " ] => [ " << it->second << " ]\n";
                    }

                currentServer.addLocation(loc);
            } else {
                std::cerr << "⚠️ Warning: Unknown directive '" << key << "'" << std::endl;
            }
        } else {
            std::cerr << "⚠️ Warning: Ignoring line outside server block: " << line << std::endl;
        }
    }
    
    if (inServerBlock)
    throw std::runtime_error("Unclosed server block");
    
    
    return servers;
}
