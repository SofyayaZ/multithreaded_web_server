#include "send_response.hpp"

#include "server/socket.hpp"
#include "response_serializer.hpp"
#include "http_response.hpp"
#include <sys/socket.h>
#include <iostream>


bool sendAll(int clientSocket, const std::string& response) {
    struct timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Socket option setting failed\n";
        return false;
    }

    ssize_t sentData = 0;
    size_t totalSent = 0;
    while (totalSent < response.size()) {
        if ( (sentData = send(clientSocket,
                         response.data() + totalSent,
                         response.size() - totalSent,
                         0)) <= 0) {
    
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "The time for sending the response to client ";
                std::cout << clientSocket << " has expired\n";
            } else {
                std::cerr << "The sending to client " << clientSocket << " failed\n";
            }
            return false;
        }
        totalSent += sentData; 
    }

    return true;
};

void sendResponse(const Socket& clientSocket, 
                  const ResponseSerializer& serializer, 
                  const HttpResponse& response) {
    // Serializing response to std::string and
    // sending it to client
    std::string serializedResponse = serializer.serializeResponse(response);
    if (!sendAll(clientSocket.get(), serializedResponse)) {
        std::cerr << "Response sending failed\n";
        return;
    }
    std::cout << "Message has been succesfully sent to client " << clientSocket.get() << "\n";
}
