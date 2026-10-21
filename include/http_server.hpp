#pragma once

#include "app_interface/application_handler.hpp"
#include <string>
#include <memory>


class Router;

class HttpServer {
    std::unique_ptr<Router> router;
    int port = 8080;
    bool flagStop = false;
public:
    HttpServer();
    ~HttpServer();

    // to start server
    int run(int port);
    // methods for configuring server 
    void get(std::string enpoint, ApplicationHandler handler);
    void post(std::string enpoint, ApplicationHandler handler);
    // for stopping server
    void stop();
};