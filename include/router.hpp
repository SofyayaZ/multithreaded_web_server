#include <vector>
#include <http_request.hpp>
#include <http_response.hpp>
#include <route.hpp>


#pragma once
class Router {
    std::vector<Route> routes;
public:
    void addRoute(std::string method, std::string endpoint, std::function<HttpResponse(const HttpRequest&)> handler);
    HttpResponse route(const HttpRequest& request);
};