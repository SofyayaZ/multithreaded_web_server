#include <client_handler.hpp>


void handleClient(const Socket& clientSocket,
                  const Router& router, 
                  const ResponseSerializer& serializer) {
    // For timeout handling
    struct timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(clientSocket.get(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Socket options set failed\n";
        return;
    }

    // For receiving data in buffers and putting them in request
    char buffer[4096];
    ssize_t receivedBytes = 0;
    std::string request;
    constexpr size_t MAX_REQUEST_SIZE = 8192;
    // Getting request part by part
    while(request.find("\r\n\r\n") == std::string::npos) {
        if ( (receivedBytes = recv(clientSocket.get(),
                                   buffer,
                                   sizeof(buffer),
                                   0)) <= 0 ) {
            if (receivedBytes < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    std::cout << "ClientSocket " << clientSocket.get() << " stopped receiving after timeout\n";
                } else {
                    perror("Failed receiving data from client\n");
                }
            } else {
                std::cerr << "Client has closed the connection\n";
            }
            
            return;
        }
        request.append(buffer, receivedBytes);
        if (request.size() > MAX_REQUEST_SIZE) {
            std::cerr << "Too long request from client " << clientSocket.get() << "\n";
            return;
        }
    }

    HttpResponse response{};

    // Request parsing for bytes from buffer
    auto httpRequest = parseRequest(request);
    if (!httpRequest) {
        std::cerr << "Bad http request structure\n";
        response.statusCode = 400;
    }
    else {
        constexpr size_t MAX_BODY_SIZE = 8192;
        if (httpRequest->contentLength > MAX_BODY_SIZE) {
            std::cerr << "Bad http request structure\n";
            response.statusCode = 400;
        }
        else {
            while (httpRequest->body.size() < httpRequest->contentLength) {
                if ( (receivedBytes = recv(clientSocket.get(), buffer, sizeof(buffer), 0)) <= 0 ) {
                    if (receivedBytes < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            std::cout << "ClientSocket " << clientSocket.get() << " stopped receiving after timeout\n";
                        } else {
                            perror("Failed receiving request body from client\n");
                        }
                    } else {
                        std::cerr << "Client has closed the connection\n";
                    }
                    
                    return;
                }
                httpRequest->body.append(buffer, receivedBytes);
                if (httpRequest->body.size() > httpRequest->contentLength ||
                    httpRequest->body.size() > MAX_BODY_SIZE) {
                    std::cerr << "Bad http request structure\n";
                    response.statusCode = 400;
                    break;
                }
            }
        }
    }

    // Routing request from client
    if (response.statusCode != 400){
        response = router.route(*httpRequest);
    }     

    // Serializing response to string for sending data to client
    std::string serializedResponse = serializer.serializeResponse(response);

    // Sending response to client
    if (!sendAll(clientSocket.get(), serializedResponse)) {
        std::cerr << "Response sending failed\n";
        return;
    }

    std::cout << "Message succesfully sent to client " << clientSocket.get() << "\n";
    return;
}