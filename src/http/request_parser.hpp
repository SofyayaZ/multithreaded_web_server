#pragma once

#include "http_request.hpp"
#include <optional>
#include <string>


bool isValidHeader(const std::string& header);
std::optional<HttpRequest> parseRequest(const std::string& request);