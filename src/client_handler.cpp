#include <client_handler.hpp>

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <request_parser.hpp>
#include <http_response.hpp>
#include <send_all.hpp>
#include <chrono>
#include <send_response.hpp>


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

    /////////////////////////////////////////
    // Recving data from client and        //
    // putting it into std::string request //
    /////////////////////////////////////////

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
            sendResponse(clientSocket, serializer, response);
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

    //////////////////////////////////
    // Parsing std::string request  //
    // Here we can get std::nullopt //
    // or HttpRequest               //
    //////////////////////////////////

    // Request parsing for bytes from buffer
    auto httpRequest = parseRequest(request);
    if (!httpRequest) {
        std::cerr << "Bad http request structure\n";
        response.statusCode = 400;
        sendResponse(clientSocket, serializer, response);
        return;
    }
    else {
        // Check parsed request body size
        if (httpRequest->contentLength > MAX_BODY_SIZE) {
            std::cerr << "Request entity too much";
            response.statusCode = 413;
            sendResponse(clientSocket, serializer, response);
            return;
        }
        else if (httpRequest->body.size() > httpRequest->contentLength) {
            std::cerr << "Bad http request structure\n";
            response.statusCode = 400;
            sendResponse(clientSocket, serializer, response);
            return;
        }
        else {
            /////////////////////////////////////////////////////////////
            // Recving body if and while it's less than Content-Length //
            /////////////////////////////////////////////////////////////
            while (httpRequest->body.size() < httpRequest->contentLength) {
                auto receivedData = recvData(clientSocket, buffer, sizeof(buffer), receivedBytes, deadline);
                if (receivedData == RecvStatus::ERROR) {
                    perror("Failed receiving data from client");
                    return;
                }
                if (receivedData == RecvStatus::TIMEOUT) {
                    std::cerr << "Request timeout\n";
                    response.statusCode = 408;
                    sendResponse(clientSocket, serializer, response);
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
                    sendResponse(clientSocket, serializer, response);
                    return;
                }
                httpRequest->body.append(buffer, receivedBytes);
            }
        }
    }

    /////////////////////////////////
    // Routing request from client //
    /////////////////////////////////
    if (httpRequest) {
        response = router.route(*httpRequest);
    }

    ////////////////////////////////
    // Sending response to client //
    ////////////////////////////////
    sendResponse(clientSocket, serializer, response);
    return;
}