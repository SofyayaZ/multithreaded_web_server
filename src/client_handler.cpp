#include <client_handler.hpp>


// -1 -> error
//  1 -> success
bool handleClient(int clientSocket, Router& router, ResponseSerializer& serializer) {
    char buffer [4096];
    ssize_t receivedBytes = 0;
    if ( (receivedBytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) <= 0 ) {
        if (receivedBytes < 0) {
            perror("Failed receiving data from client\n");
        } else {
            std::cerr << "Client has closed the connection\n";
        }
        close(clientSocket);
        return false;
    }
    buffer[receivedBytes] = '\0';

    // Request parsing for bytes from buffer
    auto httpRequest = parseRequest(buffer);
    if (!httpRequest) {
        std::cerr << "Bad http request structure\n";
        close(clientSocket);
        return false;
    }

    // Routing request from client
    HttpResponse response = router.route(*httpRequest);

    // Serializing response to string for sending data to client
    std::string serializedResponse = serializer.serializeResponse(response);

    // Sending response to client
    if (!sendAll(clientSocket, serializedResponse)) {
        std::cerr << "Response sending failed\n";
        close(clientSocket);
        return false;
    }

    std::cout << "Message succesfully sent to client " << clientSocket << "\n";
    close(clientSocket);
    return true;
}