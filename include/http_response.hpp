#pragma once
#include <string>


struct HttpResponse {
    unsigned int statusCode;
    std::string body;
};