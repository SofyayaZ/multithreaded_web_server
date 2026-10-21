#pragma once

#include "application_request.hpp"
#include "application_response.hpp"
#include <functional>


using ApplicationHandler = 
std::function<ApplicationResponse(const ApplicationRequest& appRequest)>;