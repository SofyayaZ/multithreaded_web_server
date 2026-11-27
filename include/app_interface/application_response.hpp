#pragma once

#include "headers.hpp"
#include "http_status.hpp"
#include <string>


// struct for forming response to app (client -> server -> app)
struct ApplicationResponse {
    HttpStatus status;
    Headers headers{};
    std::string body{};
};