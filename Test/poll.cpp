#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

int main() {
    // 1. صاوب socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    // 2. صاوب address و bind()
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;  // أي IP
    addr.sin_port = htons(8080);        // port 8080

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // 3. listen()
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        return 1;
    }

    // 4. poll()
    struct pollfd fds[1];
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    int timeout = 10000; // 10 ثواني

    std::cout << "⏳ Waiting for a client on port 8080...\n";

    int ret = poll(fds, 1, timeout);
    if (ret == -1) {
        perror("poll");
    } else if (ret == 0) {
        std::cout << "⏰ Timeout: ما جا حتى client\n";
    } else {
        if (fds[0].revents & POLLIN) {
            std::cout << "📞 Client تصل بالسيرفر!\n";
            // تقدر دابا تستعمل accept() مثلا
        }
    }

    close(server_fd);
    return 0;
}
