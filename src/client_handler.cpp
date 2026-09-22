#include <client_handler.hpp>

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <request_parser.hpp>
#include <http_response.hpp>
#include <send_all.hpp>


constexpr size_t MAX_HEADER_SIZE = 8192;
constexpr size_t MAX_BODY_SIZE = 8192;

enum class RecvStatus {
    SUCCESS,
    CLOSED,
    ERROR
};

RecvStatus recvData(const Socket& clientSocket,
                    char* buffer,
                    size_t bufferSize,
                    ssize_t& receivedBytes) {
    receivedBytes = recv(clientSocket.get(),
                               buffer,
                               bufferSize,
                               0);
    if (receivedBytes < 0) {
        return RecvStatus::ERROR;
    }
    if (receivedBytes == 0) {
        return RecvStatus::CLOSED;
    }
    return RecvStatus::SUCCESS;
}

void handleClient(const Socket& clientSocket,
                  const Router& router, 
                  const ResponseSerializer& serializer) {
    // For timeout handling
    struct timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // ЗДЕСЬ надо сделать так, чтобы SO_RCVTIMEO действовал не для одного recv, а
    // для всего процесса чтения сообщения от клиента в целом
    if (setsockopt(clientSocket.get(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Socket options set failed\n";
        return;
    }

    // For receiving data in buffers and putting them in request
    char buffer[4096];
    ssize_t receivedBytes = 0;
    std::string request;
    // Getting request part by part
    while(request.find("\r\n\r\n") == std::string::npos) {
        auto receivedData = recvData(clientSocket, buffer, sizeof(buffer), receivedBytes);
        if (receivedData == RecvStatus::ERROR) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "The client " << clientSocket.get() << " stopped receiving after timeout\n";
            }
            else {
                perror("Failed receiving data from client");
            }

            return;
        }
        if (receivedData == RecvStatus::CLOSED) {
            std::cout << "The client " << clientSocket.get() << " has been closed\n";
            return;
        }
        if (request.size() + receivedBytes > MAX_HEADER_SIZE) {
            std::cerr << "Too long request from client " << clientSocket.get() << "\n";
            return;
        }
        request.append(buffer, receivedBytes);
    }

    HttpResponse response{};

    // Request parsing for bytes from buffer
    auto httpRequest = parseRequest(request);
    if (!httpRequest) {
        std::cerr << "Bad http request structure\n";
        response.statusCode = 400;
    }
    else {
        if (httpRequest->contentLength > MAX_BODY_SIZE ||
            httpRequest->body.size() > httpRequest->contentLength) {
            std::cerr << "Bad http request structure\n";
            response.statusCode = 400;
        }
        else if (httpRequest->body.size() < httpRequest->contentLength) {
            while (httpRequest->body.size() < httpRequest->contentLength) {
                auto receivedData = recvData(clientSocket, buffer, sizeof(buffer), receivedBytes);
                if (receivedData == RecvStatus::ERROR) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        std::cout << "The client " << clientSocket.get() << " stopped receiving after timeout\n";
                    }
                    else {
                        perror("Failed receiving data from client");
                    }

                    return;
                }
                if (receivedData == RecvStatus::CLOSED) {
                    std::cout << "The client " << clientSocket.get() << " has been closed\n";
                    return;
                }
                if (httpRequest->body.size() + receivedBytes > httpRequest->contentLength ||
                    httpRequest->body.size() + receivedBytes > MAX_BODY_SIZE) {
                    std::cerr << "Bad http request structure\n";
                    response.statusCode = 400;
                    break;
                }
                httpRequest->body.append(buffer, receivedBytes);
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