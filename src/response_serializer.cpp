#include <response_serializer.hpp>


std::string ResponseSerializer::serializeResponse(const HttpResponse& response) {
    std::string serializedReponse = "";
    serializedReponse += "HTTP/1.1 ";
    serializedReponse += std::to_string(response.statusCode);
    serializedReponse += " ";
    serializedReponse += reasoningText.at(response.statusCode) + "\r\n";
    serializedReponse += "Content-Length: ";
    serializedReponse += std::to_string(response.body.size()) + "\r\n";
    serializedReponse += "Content-type: text/plain\r\n";
    serializedReponse += "Connection: close\r\n";
    serializedReponse += "\r\n";
    serializedReponse += response.body;
    return serializedReponse;
};
