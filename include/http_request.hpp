#pragma once
#include <string>
#include <unordered_map>


struct HttpRequest {
    std::string httpMethod;
    std::string endpoint;
    std::string httpVersion;

    std::unordered_map<std::string, std::string> headers;

    std::string body;
    size_t contentLength = 0;

    HttpRequest () {}
    HttpRequest (std::string httpMethod, std::string endpoint, std::string httpVersion) {
        this->httpMethod = httpMethod;
        this->endpoint = endpoint;
        this->httpVersion = httpVersion;
    }
};