#pragma once

#include "http/http_request.hpp"
#include "http/http_response.hpp"
#include "route.hpp"
#include <vector>
#include <string>
#include <functional>


class Router {
    std::vector<Route> routes;
public:
    void addRoute(std::string method, std::string endpoint, std::function<HttpResponse(const HttpRequest&)> handler);
    HttpResponse route(const HttpRequest& request) const;
};