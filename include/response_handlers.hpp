#pragma once
#include <http_response.hpp>
#include <http_request.hpp>


HttpResponse homeHandler(const HttpRequest& request) {
    return {200, "This is a home page", {}};
}

HttpResponse helloHandler(const HttpRequest& request) {
    return {200, "Hello", {}};
}

HttpResponse postHelloHandler(const HttpRequest& request) {
    return {200, request.body, {}};
}