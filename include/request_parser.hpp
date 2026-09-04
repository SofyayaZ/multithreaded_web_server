#pragma once
#include <optional>
#include <string>
#include <http_request.hpp>
#include <sstream>
#include <iostream>


std::optional<HttpRequest> parseRequest(const std::string& request);