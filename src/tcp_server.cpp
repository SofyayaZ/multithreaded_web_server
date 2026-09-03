#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <optional>
#include <functional>

#include <router.hpp>


std::optional<HttpRequest> parseRequest(std::string request) {
    size_t pos = request.find("\r\n");
    if (pos != std::string::npos) {
        request = request.substr(0, pos);
    }
    std::istringstream stream(request);
    HttpRequest httpRequest;
    if (!(stream >> httpRequest.httpMethod
                 >> httpRequest.endpoint
                 >> httpRequest.httpVersion)) {
        std::cerr << "Parsing error";
        return std::nullopt;
    }

    std::string extra;
    if (stream >> extra) {
        std::cerr << "Extra words in request\n";
        return std::nullopt;
    }

    return httpRequest;
};

HttpResponse homeHandler(const HttpRequest& request) {
    return {200, "This is a home page"};
}

HttpResponse helloHandler(const HttpRequest& request) {
    return {200, "Hello"};
}

int main() {
    // Creating server socket
    int serverSocket = 0;
    if ( (serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Socket creating failed");
        return 1;
    };
    std::cout << "Socket has been created succesfully " << serverSocket << "\n"; 

    // Creating sockaddr and binding it to server socket
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if ( bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Binding failed");
        close(serverSocket);
        return 1;
    };

    std::cout << "Server socket bound to port 8080 and all IPv4 interfaces\n";

    // Set server socket listening
    if ( listen(serverSocket, 10) < 0) {
        perror("Listening failed");
        return 1;
    };

    std::cout << "Socket " << serverSocket << " is listening\n";

    // Creating address for client and accepting client connection request
    struct sockaddr clientAddress = {0};
    socklen_t len = sizeof(clientAddress);
    int clientSocket = 0;
    if ( (clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &len)) < 0) {
        perror("Client connection failed");
        return 1;
    };

    std::cout << "Client socket has been created succesfully\n";

    // Receiving bytes from client and putting to buffer
    char buffer [4096];
    ssize_t receivedBytes = 0;
    if ( (receivedBytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) < 0 ) {
        perror("Failed receiving data from client");
        return 1;
    };
    buffer[receivedBytes] = '\0';

    // Request parsing for bytes from buffer
    auto httpRequest = parseRequest(buffer);
    if (!httpRequest) {
        std::cerr << "Bad http request structure\n";
        return 1;
    }

    Router router = Router();
    router.addRoute("GET", "/", homeHandler);
    router.addRoute("GET", "/hello", helloHandler);
    HttpResponse response = router.route(*httpRequest);

    // need to make serializer that will make from handlers' returns 
    // classic HTTP-response structured string

    // if ( (send(clientSocket, response, sizeof(response), 0)) < 0 ) {
    //     std::cerr << "Response sending failed\n";
    //     return 1;
    // }
    // std::cout << "Message succesfully sent to client " << clientSocket << "\n";

    // Closing sockets
    close(clientSocket);
    std::cout << "Client socket " << clientSocket << " has been closed\n";
    
    close(serverSocket);
    std::cout << "Server socket " << serverSocket << " has been closed\n";

    return 0;
}