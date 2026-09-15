#pragma once
#include <string>
#include <unordered_map>


struct HttpResponse {
    unsigned int statusCode;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
};