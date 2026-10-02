#include <send_response.hpp>

#include <socket.hpp>
#include <response_serializer.hpp>
#include <http_response.hpp>
#include <send_all.hpp>
#include <iostream>


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
