#include <request_parser.hpp>


std::optional<HttpRequest> parseRequest(const std::string& request) {
    size_t pos = request.find("\r\n");
    std::string requestLine = request;
    if (pos != std::string::npos) {
        requestLine = requestLine.substr(0, pos);
    }
    std::istringstream stream(requestLine);
    HttpRequest httpRequest;
    if (!(stream >> httpRequest.httpMethod
                 >> httpRequest.endpoint
                 >> httpRequest.httpVersion)) {
        std::cerr << "Parsing error";
        return std::nullopt;
    }

    std::string extra;
    if (stream >> extra) {
        std::cerr << "Extra words in request\n";
        return std::nullopt;
    }

    return httpRequest;
};