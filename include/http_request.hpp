#pragma once
#include <string>
struct HttpRequest {
    std::string httpMethod;
    std::string endpoint;
    std::string httpVersion;
    HttpRequest () {}
    HttpRequest (std::string httpMethod, std::string endpoint, std::string httpVersion) {
        this->httpMethod = httpMethod;
        this->endpoint = endpoint;
        this->httpVersion = httpVersion;
    };
};