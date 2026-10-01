#include <gtest/gtest.h>
#include <sys/socket.h>
#include <thread>
#include <chrono>
#include <client_handler.hpp>
#include <socket.hpp>
#include <router.hpp>
#include <response_serializer.hpp>


class ClientHandlerTest: public ::testing::Test {
protected:
    std::optional<Socket> serverSocket;
    int clientSocket = -1;

    Router router;
    ResponseSerializer serializer;
    HttpResponse getHomeHandler(HttpRequest& request) {
        return {200, "This is home page", {}};
    }

    void sendRequest(std::string request) {
        ASSERT_EQ(
            send(clientSocket, request.data(), request.size(), 0),
            static_cast<ssize_t>(request.size())
        );

        ASSERT_TRUE(serverSocket.has_value());
    }

    void SetUp() override {
        int fds[2];
        ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
        serverSocket.emplace(fds[0]);
        clientSocket = fds[1];
    }

    void TearDown() override {
        if (clientSocket != -1) {
            close(clientSocket);
            clientSocket = -1;
        }
    }
};


TEST_F(ClientHandlerTest, HandleGETValid) {
    router.addRoute("GET", "/", [](const HttpRequest& request)->HttpResponse {
        return HttpResponse{200, request.body, {}};
    });

    const std::string request{
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 15\r\n"
        "\r\n"
        "Hello from POST"
    };

    sendRequest(request);

    handleClient(*serverSocket, router, serializer);

    char buffer[4096];
    ssize_t received = recv(clientSocket, buffer, sizeof(buffer), 0);

    ASSERT_GT(received, 0);

    const std::string response(buffer, received);

    EXPECT_NE(response.find("200"), std::string::npos);
    EXPECT_NE(response.find("Hello from POST"), std::string::npos);
}

TEST_F(ClientHandlerTest, HandlePOSTValid) {
    router.addRoute("POST", "/", [](const HttpRequest&)->HttpResponse {
        return HttpResponse{200, "Successfully test POST", {}};
    });

    const std::string request{
        "POST / HTTP/1.1\r\n"
        "Host: host\r\n"
        "User-Agent: Mozilla/156.1\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Body of POST"
    };

    sendRequest(request);

    handleClient(*serverSocket, router, serializer);

    char buffer[4096];
    ssize_t received = recv(clientSocket, buffer, sizeof(buffer), 0);
    ASSERT_GT(received, 0);

    const std::string response(buffer, received);

    EXPECT_NE(response.find("Successfully test POST"), std::string::npos);
}

TEST_F(ClientHandlerTest, HandlePartialBody) {
    router.addRoute("POST", "/", [](const HttpRequest&)->HttpResponse {
        return HttpResponse{200, "Successfully POSTED", {}};
    });

    const std::string firstPart{
        "POST / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "part of b"
    };
    const std::string secondPart{"ody"};

    sendRequest(firstPart);

    std::thread handleThread([&]{
        handleClient(*serverSocket, router, serializer);
    });

    ASSERT_EQ(send(clientSocket, secondPart.data(), secondPart.size(), 0),
              static_cast<ssize_t>(secondPart.size())
    );

    handleThread.join();

    char buffer[4096];
    ssize_t receivedBytes = recv(clientSocket, buffer, sizeof(buffer), 0);

    const std::string response(buffer, receivedBytes);
    EXPECT_NE(response.find("200"), std::string::npos);
    EXPECT_NE(response.find("Successfully POSTED"), std::string::npos);
}

TEST_F(ClientHandlerTest, HandleBodyIsBiggerThanContentLength) {
    std::string request{
        "POST / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "Body"
    };

    sendRequest(request);

    handleClient(*serverSocket, router, serializer);

    char buffer[4096];
    ssize_t receivedBytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    std::string response(buffer, receivedBytes);

    EXPECT_NE(response.find("400"), std::string::npos);
    EXPECT_NE(response.find("Bad request"), std::string::npos);
}

TEST_F(ClientHandlerTest, HandleContentLengthIsBiggerThanLimit) {
    std::string request{
        "POST / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 9999\r\n"
        "\r\n"
        "Body"
    };

    sendRequest(request);

    handleClient(*serverSocket, router, serializer);
    
    char buffer[4096];
    ssize_t receivedBytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    std::string response(buffer, receivedBytes);

    EXPECT_NE(response.find("413"), std::string::npos);
    EXPECT_NE(response.find("Request Entity Too Large"), std::string::npos);
}

TEST_F(ClientHandlerTest, HandleUnknownEndpoint) {
    router.addRoute("GET", "/", [](const HttpRequest&)->HttpResponse {
        return HttpResponse{200, "Hello", {}};
    });
    std::string request{
        "GET /unknown_endpoint HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    };

    sendRequest(request);

    handleClient(*serverSocket, router, serializer);

    char buffer[4096];
    ssize_t receivedBytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    std::string response(buffer, receivedBytes);

    EXPECT_NE(response.find("404"), std::string::npos);
    EXPECT_NE(response.find("Not Found"), std::string::npos);
}

TEST_F(ClientHandlerTest, HandlePartialUnsupportedMethod) {
    router.addRoute("GET", "/", [](const HttpRequest&)->HttpResponse {
        return HttpResponse{200, "Hello", {}};
    });
    std::string request{
        "POST / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "Hi"
    };

    sendRequest(request);

    handleClient(*serverSocket, router, serializer);

    char buffer[4096];
    ssize_t receivedBytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    std::string response(buffer, receivedBytes);

    EXPECT_NE(response.find("405"), std::string::npos);
    EXPECT_NE(response.find("Method Not Allowed"), std::string::npos);
}

TEST_F(ClientHandlerTest, HandleClientClosesEarlier) {
    router.addRoute("GET", "/", [](const HttpRequest&)->HttpResponse {
        return HttpResponse{200, "You will never get this page", {}};
    });

    const std::string firstPart{
        "POST / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 10\r\n"
        "\r\n"
        "first "
    };

    sendRequest(firstPart);

    std::stringstream cout_buf;
    std::streambuf* old_cout = std::cout.rdbuf(cout_buf.rdbuf());

    std::thread handleThread([&] {
        handleClient(*serverSocket, router, serializer);
    });

    close(clientSocket);
    clientSocket = -1;

    handleThread.join();
    std::cout.rdbuf(old_cout);

    std::string message{"The client " + std::to_string(serverSocket->get()) + " has been closed\n"};

    EXPECT_EQ(cout_buf.str(), message);
}

TEST_F(ClientHandlerTest, HandleReceiveTimeout) {
    router.addRoute("GET", "/", [](const HttpRequest&)->HttpResponse {
        return HttpResponse{200, "You will never get this page", {}};
    });

    const std::string partialRequest{"GET / HTTP/1.1\r\n"};

    std::thread handleThread([&] {
        handleClient(*serverSocket, router, serializer);
    });

    sendRequest(partialRequest);

    handleThread.join();

    char buffer[4096];
    ssize_t received = recv(clientSocket, buffer, sizeof(buffer), 0);

    const std::string response(buffer, received);

    EXPECT_NE(response.find("408"), std::string::npos);
    EXPECT_NE(response.find("Request Timeout\r\n"), std::string::npos);
}
