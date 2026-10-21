#pragma once

#include "http/http_response.hpp"
#include "http/http_request.hpp"
#include <string>
#include <functional>


struct Route {
    std::string method;
    std::string endpoint;
    std::function<HttpResponse(const HttpRequest&)> handler;
    Route() {}
    Route(std::string method, std::string endpoint, std::function<HttpResponse(const HttpRequest&)> handler) {
        this->method = method;
        this->endpoint = endpoint;
        this->handler = handler;
    }
};