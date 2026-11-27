#include "http_status.hpp"
#include "response_serializer.hpp"


std::string ResponseSerializer::serializeResponse(const HttpResponse& response) const {
    std::string serializedReponse = "";
    serializedReponse += "HTTP/1.1 ";
    serializedReponse += std::to_string(codeFromStatus(response.statusCode));
    serializedReponse += " ";
    serializedReponse += reasonPhrase(response.statusCode);
    serializedReponse += "\r\n";
    serializedReponse += "Content-Length: ";
    serializedReponse += std::to_string(response.body.size()) + "\r\n";
    serializedReponse += "Content-type: text/plain\r\n";
    serializedReponse += "Connection: close\r\n";
    for (const auto& h : response.headers.entries()) {
        serializedReponse += h.name;
        serializedReponse += ": ";
        serializedReponse += h.value;
        serializedReponse += "\r\n";
    }
    serializedReponse += "\r\n";
    serializedReponse += response.body;
    return serializedReponse;
};
