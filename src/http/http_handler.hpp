#pragma once

#include "http_request.hpp"
#include "http_response.hpp"
#include <functional>


using HttpHandler = 
std::function<HttpResponse(const HttpRequest& httpRequest)>;