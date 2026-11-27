#include "headers.hpp"
#include "http_method.hpp"
#include "request_parser.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string_view>


constexpr std::string_view trimOWS(std::string_view s) noexcept {
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) {
        s.remove_prefix(1);
    }
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) {
        s.remove_suffix(1);
    }
    return s;
}

bool isValidHeader(std::string_view header) noexcept {
    return std::all_of(header.begin(), header.end(), [](unsigned char c)->bool {
        return ((c >= 'a') && (c <='z'))  ||
               ((c >= 'A') && (c <= 'Z')) ||
               ((c >= '0') && (c <= '9')) ||
               (c == '!') ||
               (c == '#') ||
               (c == '$') ||
               (c == '%') ||
               (c == '&') ||
               (c == '\'')||
               (c == '*') ||
               (c == '+') ||
               (c == '-') ||
               (c == '.') ||
               (c == '^') ||
               (c == '_') ||
               (c == '`') ||
               (c == '|') ||
               (c == '~') ;
    });
}

bool isValidHeaderValue(std::string_view value) noexcept {
    for (unsigned char c : value) {
        if (c == '\r' || c == '\n' || c == '\0') return false;  // forbidden chars for header value
        if (c < 0x20 && c != '\t')               return false;  // CTRL-combinations (^C, ^H, etc.)
        if (c == 0x7F)                           return false;  // DEL
    }
    return true;
}

std::optional<HttpRequest> parseRequest(const std::string& stringRequest) {
    std::istringstream requestStream(stringRequest);
    std::string requestLine{};

    // Getting the first line of request
    std::getline(requestStream, requestLine);
    std::istringstream stream(requestLine);
    HttpRequest httpRequest{};
    std::string strMethod;
    if (!(stream >> strMethod 
                 >> httpRequest.endpoint
                 >> httpRequest.httpVersion)) {
        std::cerr << "Parsing error\n";
        return std::nullopt;
    }

    auto httpMethod = String_toHttpMethod(strMethod);
    if (!httpMethod) {
        std::cerr << "Not active HTTP method in request: " << strMethod << "\n";
        return std::nullopt;
    }

    httpRequest.httpMethod = httpMethod.value();

    std::string extra;
    if (stream >> extra) {
        std::cerr << "Extra symbols in request: " << extra << "\n";
        return std::nullopt;
    }

    // Check HTTP request version. Valid version is HTTP/1.1
    if (httpRequest.httpVersion != "HTTP/1.1") {
        std::cerr << "Invalid HTTP request version: " << httpRequest.httpVersion << "\n";
        return std::nullopt;
    }

    // Extracting the headers
    bool endOfHeaders = false;
    while (std::getline(requestStream, requestLine, '\n')) {
        // Check the string beetwen headers and body
        if (requestLine == "\r") {
            endOfHeaders = true;
            break;
        }

        // Take a view of a request line
        std::string_view line{requestLine};
        if (line.empty() || line.back() != '\r') {
            std::cerr << "Invalid CRLF\n";
            return std::nullopt;
        }
        line.remove_suffix(1); // remove '\r' at the end of line

        // Looking for colon beetwen header and its content
        auto colon = line.find(':');
        if (colon == std::string_view::npos) {
            std::cerr << "Header string without a colon is invalid\n";
            return std::nullopt;
        }

        // Getting Header and its Content
        std::string_view header = line.substr(0, colon);
        std::string_view headerContent = trimOWS(line.substr(colon + 1));

        // Check if header is empty
        if (header.empty()) {
            std::cerr << "Empty header\n";
            return std::nullopt;
        }

        // Check if header is valid
        if (!isValidHeader(header)) {
            std::cerr << "Invalid header format\n";
            return std::nullopt;
        }

        if (!isValidHeaderValue(headerContent)) {
            std::cerr << "Invalid header content format\n";
            return std::nullopt;
        }

        httpRequest.headers.add(std::string(header), std::string(headerContent));
    }

    // Check double headers for Host, User-Agent, Content-Length
    if (httpRequest.headers.getAll("Host").size() > 1 ||
        httpRequest.headers.getAll("User-Agent").size() > 1 ||
        httpRequest.headers.getAll("Content-Length").size() > 1) {
        std::cerr << "Duplicated Host / User-Agent / Content-Length is invalid\n";
        return std::nullopt;
    }

    if (!endOfHeaders) {
        std::cerr << "End of headers not found\n";
        return std::nullopt;
    }

    // Check no "Transfer-Encoding" - for now
    if (httpRequest.headers.contains("transfer-encoding")) {
        std::cerr << "Transfer-Encoding => ban\n";
        return std::nullopt;
    }

    // Host is a necessary header
    if (!httpRequest.headers.contains("host")) {
        std::cerr << "Host header is invalid header format\n";
        return std::nullopt;
    }

    // Check host not empty
    if (httpRequest.headers.get("host") == "") {
        std::cerr << "Host cannot be empty\n";
        return std::nullopt;
    }

    // Getting the content length
    if (httpRequest.headers.contains("content-length")) {
        auto contentLengthValue = httpRequest.headers.get("content-length");
        if (!contentLengthValue) {
            std::cerr << "No Content-Length value found\n";
            return std::nullopt;
        }
        
        for (unsigned char c : *contentLengthValue) {
            if (!std::isdigit(c)) {
                std::cerr << "Invalid Content-Length value\n";
                return std::nullopt;
            }
        }

        std::istringstream contentLengthStream(*contentLengthValue);
        if (!(contentLengthStream >> httpRequest.contentLength)) {
            std::cerr << "Getting Content-Length from stream failed\n";
            return std::nullopt;
        }
    }

    // Check body
    auto bodyPosition = stringRequest.find("\r\n\r\n");
    if (bodyPosition == std::string::npos) {
        std::cerr << "The empty line beetwen headers and body is not found\n";
        return std::nullopt;
    }
    
    httpRequest.body = stringRequest.substr(bodyPosition + 4);

    if (!httpRequest.headers.contains("content-length") && !httpRequest.body.empty()) {
        std::cerr << "Need Content-Length\n";
        return std::nullopt;
    }

    if (httpRequest.body.size() > httpRequest.contentLength) {
        std::cerr << "Real body size is bigger than the Content-Length\n";
        return std::nullopt;
    }

    return httpRequest;
}