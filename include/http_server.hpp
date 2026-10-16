#pragma once

#include <http_server.hpp>
#include <router.hpp>
#include <application_request.hpp>
#include <application_response.hpp>
#include <functional>
#include <string>


class HttpServer {
    Router router;
    int port = 8080;
    bool flagStop = false;
public:
    // to start server
    int run(int port);
    // methods for configuring server 
    void get(std::string enpoint, std::function<ApplicationResponse(const ApplicationRequest& request)> handler);
    void post(std::string enpoint, std::function<ApplicationResponse(const ApplicationRequest& request)> handler);
    // for stopping server
    void stop();
};