#pragma once

#include "http_request.hpp"
#include <optional>
#include <string>


std::optional<HttpRequest> parseRequest(const std::string& request);