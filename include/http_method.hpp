#pragma once

#include <optional>
#include <string_view>


enum class HttpMethod {
    GET,
    HEAD,
    OPTIONS,
    POST,
    PATCH,
    PUT,
    DELETE
};

constexpr std::string_view HttpMethod_toString(HttpMethod method) noexcept {
    switch (method) {
        case HttpMethod::GET:       return "GET";
        case HttpMethod::HEAD:      return "HEAD";
        case HttpMethod::OPTIONS:   return "OPTIONS";
        case HttpMethod::POST:      return "POST";
        case HttpMethod::PATCH:     return "PATCH";
        case HttpMethod::PUT:       return "PUT";
        case HttpMethod::DELETE:    return "DELETE";
    }
    return {};
}

constexpr std::optional<HttpMethod> String_toHttpMethod(std::string_view strMethod) noexcept {
    if (strMethod == "GET")         return HttpMethod::GET;
    if (strMethod == "HEAD")        return HttpMethod::HEAD;
    if (strMethod == "OPTIONS")     return HttpMethod::OPTIONS;
    if (strMethod == "POST")        return HttpMethod::POST;
    if (strMethod == "PATCH")       return HttpMethod::PATCH;
    if (strMethod == "PUT")         return HttpMethod::PUT;
    if (strMethod == "DELETE")      return HttpMethod::DELETE;
    return std::nullopt;
}