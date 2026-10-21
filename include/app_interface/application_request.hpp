#pragma once

#include "headers.hpp"
#include <string>
#include <unordered_map>


enum class HttpMethod {
    GET,
    HEAD,
    OPTIONS,
    POST,
    PATCH,
    PUT,
    DELETE
};

// struct for forming request from app to server
struct ApplicationRequest {
    HttpMethod method;
    std::string path;
    Headers headers{};
    std::string body{};
};