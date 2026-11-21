#pragma once

#include "app_interface/application_handler.hpp"
#include "app_interface/server_config.hpp"
#include <atomic>
#include <memory>
#include <string>


class Router;

class HttpServer {
    ServerConfig config;
    std::atomic<bool> flagStop{false};
    std::unique_ptr<Router> router;
public:
    HttpServer(ServerConfig serverConfig = {});
    ~HttpServer();

    // to start server
    int run();
    // methods for configuring server
    void get(const std::string& endpoint, ApplicationHandler handler);
    void post(const std::string& endpoint, ApplicationHandler handler);
    // for stopping server
    void stop();
};
