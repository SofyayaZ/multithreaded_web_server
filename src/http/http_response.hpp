#pragma once

#include "headers.hpp"
#include "http_status.hpp"
#include <string>


struct HttpResponse {
    HttpStatus statusCode = HttpStatus::InternalServerError;
    std::string body;
    Headers headers;

    HttpResponse() = default;
    HttpResponse(HttpStatus statusCode, std::string body, Headers headers):
        statusCode(statusCode),
        body(std::move(body)),
        headers(std::move(headers)) {}
};