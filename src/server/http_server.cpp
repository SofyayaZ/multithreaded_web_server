#include "http_server.hpp"

#include "app_interface/application_handler.hpp"
#include "http/http_handler.hpp"
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <optional>
#include <functional>
#include <thread>
#include <memory>

#include "routing/router.hpp"
#include "http/request_parser.hpp"
#include "http/response_serializer.hpp"
#include "client_handler.hpp"
#include "thread_pool.hpp"
#include "socket.hpp"


HttpServer::HttpServer() : router(std::make_unique<Router>()) {}

HttpServer::~HttpServer() = default;

int HttpServer::run (int port) {
    // For handling POSIX-signals
    // struct sigaction sa{};
    // sa.sa_handler = [&](int signal) {
    //     if (signal == SIGINT) {
    //         flagStop = 1;
    //     }
    // };
    // sigemptyset(&sa.sa_mask);     // do not block extra signals
    // sa.sa_flags = 0;

    // if (sigaction(SIGINT, &sa, NULL) == -1) {
    //     perror("Failed to call sigaction");
    //     return 1;
    // }

    // Creating server socket
    int serverFd = 0;
    if ( (serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Socket creating failed\n");
        return 1;
    }
    std::cout << "Socket has been created succesfully " << serverFd << "\n"; 

    // Reuse address in OS
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsocket failed\n");
        close(serverFd);
        return 1;
    }

    // Creating sockaddr and binding it to server socket
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverFd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        perror("Binding failed\n");
        close(serverFd);
        std::cout << "Server socket " << serverFd << " has been closed\n";
        return 1;
    }

    std::cout << "Server socket bound to port " << port << " and all IPv4 interfaces\n";

    // Set server socket listening
    if (listen(serverFd, 10) < 0) {
        perror("Listening failed\n");
        close(serverFd);
        std::cout << "Server socket " << serverFd << " has been closed\n";
        return 1;
    }

    std::cout << "Socket " << serverFd << " is listening\n";

    //Creating serializer (HttpResponse -> std::string)
    ResponseSerializer serializer = ResponseSerializer();

    // ThreadPool for accepting clients' connections
    ThreadPool threadPool = ThreadPool(4, *router, serializer);

    // Handling client's requests
    while(!flagStop) {
        // Creating address for client and accepting client connection request
        sockaddr clientAddress{};
        socklen_t len = sizeof(clientAddress);
        int clientFd = 0;

        if ( (clientFd = accept(serverFd, &clientAddress, &len)) < 0) {
            if (errno == EINTR && flagStop) {
                break;
            }
            std::cerr << "Failed to accept client connection\n";
            continue;
        }
        Socket clientSocket(clientFd);
        std::cout << "Client socket has been created successfully\n";
        
        // Copying of Socket is forbidden. Moving resources
        threadPool.enqueue(std::move(clientSocket));
    }

    // Closing server socket
    close(serverFd);
    std::cout << "Server socket " << serverFd << " has been closed\n";

    return 0;
}

void HttpServer::get(
    std::string endpoint, 
    ApplicationHandler appHandler
) {
    
    // парсинг appHandler приложения в httpHandler
    // HttpHadnler httpHandler = adapt(appHandler);
    // router->addRoute("GET", endpoint, httpHandler);
}

void HttpServer::post(
    std::string endpoint, 
    ApplicationHandler appHandler
) {

    // парсинг appHandler приложения в httpHandler
    // HttpHandler httpHandler = adapt(appHandler);
    // router->addRoute("POST", endpoint, httpHandler);
}

void HttpServer::stop() {
    this->flagStop = true;

    // здесь пока ещё не произойдёт полноценной остановки программы...
}