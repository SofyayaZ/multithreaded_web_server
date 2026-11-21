#pragma once

#include "app_interface/application_handler.hpp"
#include "app_interface/application_request.hpp"
#include "http/http_handler.hpp"
#include "http/http_response.hpp"


HttpHandler adaptHandler(ApplicationHandler appHandler);
