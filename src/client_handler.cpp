#include <client_handler.hpp>

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <request_parser.hpp>
#include <http_response.hpp>
#include <send_all.hpp>
#include <chrono>


constexpr size_t MAX_HEADER_SIZE = 8192;
constexpr size_t MAX_BODY_SIZE = 8192;
constexpr auto REQUEST_RECV_TIMEOUT = std::chrono::seconds(60);

enum class RecvStatus {
    SUCCESS,
    CLOSED,
    TIMEOUT,
    ERROR
};

using Clock = std::chrono::steady_clock;

RecvStatus recvData(const Socket& clientSocket,
                    char* buffer,
                    size_t bufferSize,
                    ssize_t& receivedBytes,
                    Clock::time_point deadline) {
    auto remainingTime = 
        std::chrono::duration_cast<std::chrono::microseconds>(deadline - Clock::now());
    if (remainingTime.count() <= 0) {
        return RecvStatus::TIMEOUT;
    }

    struct timeval timeout{};
    timeout.tv_sec = remainingTime.count() / 1000000;
    timeout.tv_usec = remainingTime.count() % 1000000;

    if (setsockopt(clientSocket.get(), 
                   SOL_SOCKET, SO_RCVTIMEO, 
                   &timeout, 
                   sizeof(timeout)) < 0) {
        std::cerr << "Socket options set failed\n";
        return RecvStatus::ERROR;
    }

    receivedBytes = recv(clientSocket.get(),
                         buffer,
                         bufferSize,
                         0);
    if (receivedBytes < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return RecvStatus::TIMEOUT;
        }
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
    // For timeout handling set deadline
    auto deadline = Clock::now() + REQUEST_RECV_TIMEOUT;

    // For receiving data in buffers and putting them in request
    char buffer[4096];
    ssize_t receivedBytes = 0;
    std::string request;
    HttpResponse response{};
    // Getting request part by part
    while(request.find("\r\n\r\n") == std::string::npos) {
        auto receivedData = recvData(clientSocket,
                                     buffer, 
                                     sizeof(buffer), 
                                     receivedBytes, 
                                     deadline);
        if (receivedData == RecvStatus::ERROR) {
            perror("Failed receiving data from client");
            return;
        }
        if (receivedData == RecvStatus::TIMEOUT) {
            std::cerr << "Request timeout\n";
            response.statusCode = 408;
            break;
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

    // Это костыль ... 
    if (response.statusCode == 408) {
        std::string serializedResponse = serializer.serializeResponse(response);
        if (!sendAll(clientSocket.get(), serializedResponse)) {
            std::cerr << "Response sending failed\n";
            return;
        }
        std::cout << "Message succesfully sent to client " << clientSocket.get() << "\n";
    }

    // Request parsing for bytes from buffer
    auto httpRequest = parseRequest(request);
    if (!httpRequest) {
        std::cerr << "Bad http request structure\n";
        response.statusCode = 400;
    }
    else {
        if (httpRequest->contentLength > MAX_BODY_SIZE) {
            std::cerr << "Request entity too much";
            response.statusCode = 413;
        }
        else if (httpRequest->body.size() > httpRequest->contentLength) {
            std::cerr << "Bad http request structure\n";
            response.statusCode = 400;
        }
        else if (httpRequest->body.size() < httpRequest->contentLength) {
            while (httpRequest->body.size() < httpRequest->contentLength) {
                auto receivedData = recvData(clientSocket, buffer, sizeof(buffer), receivedBytes, deadline);
                if (receivedData == RecvStatus::ERROR) {
                    perror("Failed receiving data from client");
                    return;
                }
                if (receivedData == RecvStatus::TIMEOUT) {
                    std::cerr << "Request timeout\n";
                    response.statusCode = 408;
                    break;
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
    if (httpRequest) {
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