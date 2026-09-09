#include <client_handler.hpp>


bool handleClient(int clientSocket, Router& router, ResponseSerializer& serializer) {
    // For timeout handling
    struct timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Socket options set failed\n";
        close(clientSocket);
        return false;
    }

    // For receiving data in buffers and putting them in request
    char buffer [4096];
    ssize_t receivedBytes = 0;
    std::string request;
    constexpr size_t MAX_REQUEST_SIZE = 8192;

    while(request.find("\r\n\r\n") == std::string::npos) {
        if ( (receivedBytes = recv(clientSocket,
                                   buffer,
                                   sizeof(buffer) - 1,
                                   0)) <= 0 ) {
            if (receivedBytes < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    std::cout << "ClientSocket " << clientSocket << " stopped receiving after timeout\n";
                } else {
                    perror("Failed receiving data from client\n");
                }
            } else {
                std::cerr << "Client has closed the connection\n";
            }
            close(clientSocket);
            return false;
        }
        request.append(buffer, receivedBytes);
        if (request.size() > MAX_REQUEST_SIZE) {
            std::cerr << "Too long request from client " << clientSocket << "\n";
            close(clientSocket);
            return false;
        }
    }

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