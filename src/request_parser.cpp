#include <request_parser.hpp>

#include <sstream>
#include <iostream>


std::optional<HttpRequest> parseRequest(const std::string& stringRequest) {
    std::stringstream requestStream(stringRequest);
    std::string requestLine{};

    // Getting the first line of request
    std::getline(requestStream, requestLine);
    std::istringstream stream(requestLine);
    HttpRequest httpRequest{};
    if (!(stream >> httpRequest.httpMethod
                 >> httpRequest.endpoint
                 >> httpRequest.httpVersion)) {
        std::cerr << "Parsing error\n";
        return std::nullopt;
    }

    // Getting the headers
    while (std::getline(requestStream, requestLine)) {
        // Check the string beetwen headers and body
        if (requestLine == "\r") {
            break;
        }

        std::string header;
        std::string headerContent;

        // Finding colon beetwen header and its content
        auto colon = requestLine.find(':');
        if (colon == std::string::npos) {
            std::cerr << "Header without a colon is invalid\n";
            return std::nullopt;
        }

        // Getting header and its content
        header = requestLine.substr(0, colon);
        headerContent = requestLine.substr(colon + 1);

        // Deleting '\r' from header content
        if (!headerContent.empty() && headerContent.back() == '\r') {
            headerContent.pop_back();
        }
        // Deleting ' ' from header content
        auto first = headerContent.find_first_not_of(' ');
        if (first != std::string::npos) {
            headerContent.erase(0, first);
        }
        httpRequest.headers[header] = headerContent;
    }

    // Getting the content length
    auto it = httpRequest.headers.find("Content-Length");
    if (it == httpRequest.headers.end()) {
        std::cout << "No body in request\n";
        return httpRequest;
    }
    
    std::istringstream contentLengthStream(it->second);
    if (!(contentLengthStream >> httpRequest.contentLength)) {
        std::cerr << "Getting content-length from stream failed\n";
        return std::nullopt;
    }

    // Check no extra words in content length
    std::string extra;
    if (contentLengthStream >> extra) {
        std::cerr << extra << " -- extra words in content-length\n";
        return std::nullopt;
    }

    auto bodyPosition = stringRequest.find("\r\n\r\n");
    if (bodyPosition == std::string::npos) {
        if (httpRequest.contentLength == 0) {
            return httpRequest;
        }
        std::cerr << "The empty line beetwen headers and body is not found\n";
        return std::nullopt;
    }
    httpRequest.body = stringRequest.substr(bodyPosition + 4);

    if (httpRequest.body.size() > httpRequest.contentLength) {
        std::cerr << "Real body size is bigger than the Content-Length\n";
        return std::nullopt;
    }

    return httpRequest;
}