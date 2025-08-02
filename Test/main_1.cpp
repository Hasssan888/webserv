#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // int yes = 1;
    // setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 10);

    std::cout << "Server listening on 127.0.0.1:8080\n";

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        std::cout << "Client connected!\n";

        // نقرأ من client (ممكن تتجاهل المحتوى)
        char buffer[1024];
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer)-1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::cout << "Received:\n" << buffer << std::endl;
        }

        // نرسل رد HTTP بسيط
        const char* http_response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 13\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello, world!";

        write(client_fd, http_response, strlen(http_response));

        close(client_fd);
        std::cout << "Client disconnected.\n";
    }

    close(sockfd);
    return 0;
}
