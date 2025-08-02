#include <iostream>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class SimpleServer {
private:
    int _fd;
    std::string _host;
    int _port;

public:
    SimpleServer(const std::string& host, int port)
        : _fd(-1), _host(host), _port(port) {}

    ~SimpleServer() {
        if (_fd != -1)
            close(_fd);
    }

    void setup() {
        std::cout << "🔧 Setting up server on " << _host << ":" << _port << "\n";

        // Step 1: Create socket
        _fd = socket(AF_INET, SOCK_STREAM, 0);
        if (_fd < 0)
            throw std::runtime_error("❌ Failed to create socket");

        std::cout << "✅ Socket created (fd=" << _fd << ")\n";

        // Step 2: Set socket options
        // int yes = 1;
        // if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        //     throw std::runtime_error("❌ setsockopt failed");

        // std::cout << "✅ SO_REUSEADDR set\n";

        // Step 3: Prepare address
        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(_port);

        if (_host == "0.0.0.0") {
            addr.sin_addr.s_addr = INADDR_ANY;
            std::cout << "✅ Binding to all interfaces (0.0.0.0)\n";
        } else {
            addr.sin_addr.s_addr = inet_addr(_host.c_str());
            if (addr.sin_addr.s_addr == INADDR_NONE)
                throw std::runtime_error("❌ Invalid IP address");
        }

        // Step 4: Bind socket
        if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
            throw std::runtime_error("❌ Bind failed");

        std::cout << "✅ Successfully bound\n";

        // Step 5: Set non-blocking
        if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
            throw std::runtime_error("❌ Failed to set non-blocking");

        std::cout << "✅ Non-blocking mode set\n";

        // Step 6: Start listening
        if (listen(_fd, 100) < 0)
            throw std::runtime_error("❌ Listen failed");

        std::cout << "🎧 Server is now listening on " << _host << ":" << _port << "\n";
    }

    int getFd() const { return _fd; }
};

int main() {
    try {
        SimpleServer server("127.0.0.1", 8080);
        server.setup();

        std::cout << "🕒 Press Ctrl+C to exit.\n";
        while (true) {
            sleep(1);  // Just wait forever (you can add accept here)
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
