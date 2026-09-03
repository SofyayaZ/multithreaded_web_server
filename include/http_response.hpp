#pragma once
struct HttpResponse {
    unsigned int statusCode;
    std::string header;
    std::string body;
};