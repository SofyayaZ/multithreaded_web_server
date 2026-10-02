#pragma once
#include <socket.hpp>
#include <response_serializer.hpp>
#include <http_response.hpp>


void sendResponse(const Socket& clientSocket, const ResponseSerializer& serializer, const HttpResponse& response);
