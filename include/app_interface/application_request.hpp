#pragma once

#include "headers.hpp"
#include "http_method.hpp"
#include <string>


// struct for forming request from app to server
struct ApplicationRequest {
    HttpMethod method;
    std::string path;
    Headers headers{};
    std::string body{};
};