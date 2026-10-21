#pragma once

#include "app_interface/application_handler.hpp"
#include "http/http_handler.hpp"
#include "app_interface/application_request.hpp"
#include "http/http_response.hpp"
#include "http/http_response.hpp"
#include "app_interface/application_request.hpp"


HttpHandler adaptHandler(const ApplicationHandler& appHandler);
ApplicationRequest toApplicationRequest(const HttpResponse& httpResponse);
HttpResponse toHttpResponse(const ApplicationResponse& appResponse);