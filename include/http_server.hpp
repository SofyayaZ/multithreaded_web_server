#pragma once

#include "app_interface/application_handler.hpp"
#include "app_interface/server_config.hpp"
#include "http_method.hpp"
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
    // get, post, put, patch, delete_, options, head - all in one method
    void route(HttpMethod method, const std::string& endpoint, ApplicationHandler handler);
    // for stopping server
    void stop();
};
