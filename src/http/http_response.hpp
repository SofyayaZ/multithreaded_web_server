#pragma once

#include "app_interface/headers.hpp"
#include <string>


struct HttpResponse {
    unsigned int statusCode;
    std::string body;
    Headers headers;

    HttpResponse() {}
    HttpResponse(unsigned int statusCode, std::string body, Headers headers):
        statusCode(statusCode),
        body(std::move(body)),
        headers(std::move(headers)) {}
};