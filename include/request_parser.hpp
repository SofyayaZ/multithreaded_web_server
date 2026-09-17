#pragma once
#include <optional>
#include <string>
#include <http_request.hpp>


std::optional<HttpRequest> parseRequest(const std::string& request);