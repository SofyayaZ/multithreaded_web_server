#pragma once

#include "headers.hpp"
#include "http_method.hpp"
#include <string>


struct HttpRequest {
    HttpMethod httpMethod;
    std::string endpoint;
    std::string httpVersion;

    Headers headers;

    std::string body;
    unsigned int contentLength = 0;

    HttpRequest() {}
    HttpRequest(HttpMethod httpMethod, std::string endpoint, std::string httpVersion):
                httpMethod(httpMethod), endpoint(endpoint), httpVersion(httpVersion) {}
};