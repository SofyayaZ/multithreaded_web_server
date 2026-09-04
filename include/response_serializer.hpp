#pragma once
#include <string>
#include <http_response.hpp>
#include <unordered_map>


class ResponseSerializer {
    std::unordered_map<unsigned int, std::string> reasoningText = {
        {200, "OK"},
        {404, "Not Found"},
        {405, "Method Not Allowed"}
    };

public:
    std::string serializeResponse(const HttpResponse& response);
};
