#include "adapt_handler.hpp"

#include "app_interface/application_handler.hpp"
#include "app_interface/application_request.hpp"
#include "http/http_handler.hpp"
#include "http/http_response.hpp"
#include "http_status.hpp"
#include <string>
#include <unordered_map>


ApplicationRequest toApplicationRequest(const HttpRequest& httpRequest) {
    ApplicationRequest appRequest;
    appRequest.method = httpRequest.httpMethod;
    appRequest.path = httpRequest.endpoint;
    appRequest.headers = httpRequest.headers;
    appRequest.body = httpRequest.body;
    return appRequest;
}

HttpResponse toHttpResponse(const ApplicationResponse& appResponse) {
    HttpResponse httpResponse;
    httpResponse.statusCode = appResponse.status;
    httpResponse.headers = appResponse.headers;
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
