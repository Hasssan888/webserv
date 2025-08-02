// simple_cgi_server.cpp
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <cstring>
#include <fcntl.h>

#define PORT 8080

void serveClient(int clientSocket) {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    read(clientSocket, buffer, sizeof(buffer));

    // بسيط: أي طلب فيه /cgi-bin/ كنشغلو السكريبت
    std::string request(buffer);
    if (request.find("GET /cgi-bin/script.py") != std::string::npos) {
        int pipefd[2];
        pipe(pipefd);

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
            close(pipefd[0]); // Close unused read end

            setenv("REQUEST_METHOD", "GET", 1);
            execl("./cgi-bin/script.py", "script.py", NULL);

            // If execl fails
            perror("execl");
            exit(1);
        } else {
            // Parent process
            close(pipefd[1]); // Close unused write end
            waitpid(pid, NULL, 0);

            char cgi_output[4096];
            int bytesRead = read(pipefd[0], cgi_output, sizeof(cgi_output) - 1);
            cgi_output[bytesRead] = '\0';

            std::string response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: text/html\r\n\r\n";
            response += cgi_output;

            send(clientSocket, response.c_str(), response.size(), 0);
        }
    } else {
        std::string notFound = "HTTP/1.1 404 Not Found\r\n\r\nPage not found.";
        send(clientSocket, notFound.c_str(), notFound.size(), 0);
    }

    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    sockaddr_in serverAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    std::cout << "Simple CGI Server listening on port " << PORT << "...\n";

    while (true) {
        clientSocket = accept(serverSocket, NULL, NULL);
        serveClient(clientSocket);
    }

    close(serverSocket);
    return 0;
}
