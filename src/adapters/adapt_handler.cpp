#include "adapt_handler.hpp"

#include "app_interface/application_handler.hpp"
#include "app_interface/application_request.hpp"
#include "http/http_handler.hpp"
#include "http/http_response.hpp"
#include <string>
#include <unordered_map>


const std::unordered_map<std::string, HttpMethod> methods {
    {"HEAD",    HttpMethod::HEAD},
    {"OPTIONS", HttpMethod::OPTIONS},
    {"GET",     HttpMethod::GET},
    {"POST",    HttpMethod::POST},
    {"PATCH",   HttpMethod::PATCH},
    {"PUT",     HttpMethod::PUT},
    {"DELETE",  HttpMethod::DELETE}
};

const std::unordered_map<HttpStatus, unsigned int> statusCodes {
    {HttpStatus::OK,                    200},
    {HttpStatus::Created,               201},
    {HttpStatus::NoContent,             204},
    {HttpStatus::BadRequest,            400},
    {HttpStatus::Unauthorized,          401},
    {HttpStatus::NotFound,              404},
    {HttpStatus::MethodNotAllowed,      405},
    {HttpStatus::RequestTimeout,        408},
    {HttpStatus::RequestEntityTooLarge, 413},
    {HttpStatus::InternalServerError,   500},
};

ApplicationRequest toApplicationRequest(const HttpRequest& httpRequest) {
    ApplicationRequest appRequest;
    auto it = methods.find(httpRequest.httpMethod);
    if (it != methods.end()) {
        appRequest.method = it->second;
    }
    // appRequest.path = httpRequest.endpoint;
    // appRequest.headers = httpRequest.headers;
    appRequest.body = httpRequest.body;
    return appRequest;
}

HttpResponse toHttpResponse(const ApplicationResponse& appResponse) {
    HttpResponse httpResponse;
    auto it = statusCodes.find(appResponse.status);
    if (it != statusCodes.end()) {
        httpResponse.statusCode = it->second;
    }
    // httpResponse.headers = appResponse.headers;
    httpResponse.body = appResponse.body;
    return httpResponse;
}

HttpHandler adaptHandler(ApplicationHandler appHandler) {
    return [handler = std::move(appHandler)] (const HttpRequest& httpRequest) -> HttpResponse {
        ApplicationRequest appRequest = toApplicationRequest(httpRequest);
        ApplicationResponse appResponse = handler(appRequest);
        return toHttpResponse(appResponse);
    };
}
