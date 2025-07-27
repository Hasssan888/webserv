// test_HttpRequest.cpp
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

int main() {
    HttpResponse response;

    response.setStatusCode(200);
    response.setHeader("Content-Type", "text/html");
    response.setBody("<h1>Test Response</h1>");

    std::string res = response.toString();

    std::cout << "===== HTTP Response =====" << std::endl;
    std::cout << res << std::endl;

    // // نموذج طلب HTTP كامل وبسيط (request string)
    // std::string rawRequest = 
    //     "GET /index.html HTTP/1.1\r\n"
    //     "Host: localhost\r\n"
    //     "User-Agent: TestAgent\r\n"
    //     "Content-Length: 11\r\n"
    //     "\r\n"
    //     "Hello World";

    // HttpRequest request;
    // bool success = request.parse(rawRequest);

    // if (!success) {
    //     std::cout << "Parsing failed!" << std::endl;
    //     return 1;
    // }

    // // طبع النتائج للتأكد
    // std::cout << "Method: " << request.getMethod() << std::endl;
    // std::cout << "URI: " << request.getUri() << std::endl;
    // std::cout << "HTTP Version: " << request.getHttpVersion() << std::endl;

    // std::cout << "Headers:" << std::endl;
    // for (std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin();
    //      it != request.getHeaders().end(); ++it) {
    //     std::cout << "  " << it->first << ": " << it->second << std::endl;
    // }

    // std::cout << "Body: " << request.getBody() << std::endl;

    return 0;
}
