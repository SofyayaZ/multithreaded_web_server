#include <request_parser.hpp>

#include <sstream>
#include <iostream>
#include <algorithm>


bool isValidHeader(const std::string& header) {
    return std::all_of(header.begin(), header.end(), [](unsigned char c)->bool {
        return c >= 'a' && c <='z' ||
               c >= 'A' && c <= 'Z' ||
               c >= '0' && c <= '9' ||
               c == '!' ||
               c == '#' ||
               c == '$' ||
               c == '%' ||
               c == '&' ||
               c == '\''||
               c == '*' ||
               c == '+' ||
               c == '-' ||
               c == '.' ||
               c == '^' ||
               c == '_' ||
               c == '`' ||
               c == '|' ||
               c == '~';
    });
}

std::optional<HttpRequest> parseRequest(const std::string& stringRequest) {
    std::istringstream requestStream(stringRequest);
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

    // Getting the headers
    bool hasContentLength = false;
    bool hasHost = false;
    bool hasUserAgent = false;
    bool endOfHeaders = false;
    while (std::getline(requestStream, requestLine, '\n')) {
        // Check the string beetwen headers and body
        if (requestLine == "\r") {
            endOfHeaders = true;
            break;
        }

        if (requestLine.find('\r') == std::string::npos) {
            std::cerr << "Invalid CRLF\n";
            return std::nullopt;
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

        // Normalizing header
        for (auto& c : header) {
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }
        // Check double Content-Length header
        if (header == "content-length") {
            if (hasContentLength) {
                std::cerr << "Duplicated Content-Length header is invalid\n";
                return std::nullopt;
            }
            hasContentLength = true;
        }

        // Check double Host header
        if (header == "host") {
            if (hasHost) {
                std::cerr << "Duplicated Host header is invalid\n";
                return std::nullopt;
            }
            hasHost = true;
        }

        // Check double User-Agent header
        if (header == "user-agent") {
            if (hasUserAgent) {
                std::cerr << "Duplicated User-Agent header is invalid\n";
                return std::nullopt;
            }
            hasUserAgent = true;
        }

        // Extracting header content
        headerContent = requestLine.substr(colon + 1);

        // Deleting '\r' from header content
        if (!headerContent.empty() && headerContent.back() == '\r') {
            headerContent.pop_back();
        }
        // Deleting ' ' from header content, check if it's empty
        auto first = headerContent.find_first_not_of(' ');
        if (first == std::string::npos) {
            std::cerr << "Empty header content\n";
            return std::nullopt;
        }

        if (first != 0) {
            headerContent.erase(0, first);
        }

        httpRequest.headers[header] = headerContent;
    }

    if (!endOfHeaders) {
        std::cerr << "End of headers not found\n";
        return std::nullopt;
    }

    // Iterator for checking headers
    std::unordered_map<std::string, std::string>::iterator it{};

    // Check no "Transfer-Encoding"
    it = httpRequest.headers.find("transfer-encoding");
    if (it != httpRequest.headers.end()) {
        std::cerr << "Transfer-Encoding => ban\n";
        return std::nullopt;
    }

    // Find Host
    it = httpRequest.headers.find("host");
    if (it == httpRequest.headers.end()) {
        std::cerr << "Host header is invalid header format\n";
        return std::nullopt;
    }

    // Check host os not empty
    if (it->second == "") {
        std::cerr << "Host cannot be empty\n";
        return std::nullopt;
    }

    // Getting the content length
    it = httpRequest.headers.find("content-length");
    if (it != httpRequest.headers.end()) {
        const std::string& contentLengthValue = it->second;
        
        for (unsigned char c : contentLengthValue) {
            if (!std::isdigit(c)) {
                std::cerr << "Invalid Content-Length value\n";
                return std::nullopt;
            }
        }

        std::istringstream contentLengthStream(contentLengthValue);
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

    if (it == httpRequest.headers.end() && !httpRequest.body.empty()) {
        std::cerr << "Need Content-Length\n";
        return std::nullopt;
    }

    if (httpRequest.body.size() > httpRequest.contentLength) {
        std::cerr << "Real body size is bigger than the Content-Length\n";
        return std::nullopt;
    }

    return httpRequest;
}