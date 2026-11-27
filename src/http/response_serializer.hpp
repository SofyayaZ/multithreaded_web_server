#pragma once

#include "http_response.hpp"
#include <string>


class ResponseSerializer {
public:
    std::string serializeResponse(const HttpResponse& response) const;
};
