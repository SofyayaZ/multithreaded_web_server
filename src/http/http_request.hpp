#pragma once

#include "http_method.hpp"
#include <string>
#include <unordered_map>


struct HttpRequest {
    HttpMethod httpMethod;
    std::string endpoint;
    std::string httpVersion;

    std::unordered_map<std::string, std::string> headers;

    std::string body;
    unsigned int contentLength = 0;

    HttpRequest() {}
    HttpRequest(HttpMethod httpMethod, std::string endpoint, std::string httpVersion) {
        this->httpMethod = httpMethod;
        this->endpoint = endpoint;
        this->httpVersion = httpVersion;
    }
};