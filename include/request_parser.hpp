#pragma once
#include <optional>
#include <string>
#include <http_request.hpp>


bool isValidHeader(const std::string& header);
std::optional<HttpRequest> parseRequest(const std::string& request);