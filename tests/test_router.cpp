#include "headers.hpp"
#include "http_method.hpp"
#include "http_status.hpp"
#include "http/http_response.hpp"
#include "routing/router.hpp"

#include <gtest/gtest.h>


HttpResponse homeHandler(const HttpRequest& request) {
    return {HttpStatus::OK, "Home page", {}};
}

HttpResponse helloHandler(const HttpRequest& request) {
    return {HttpStatus::OK, "Hello", {}};
}

HttpResponse postHelloHandler(const HttpRequest& request) {
    return {HttpStatus::OK, "Hello from POST", {}};
}

TEST(RouterTest, GetExistingRouteReturnsOK) {
    // Added router with GET route for test
    Router router;
    router.addRoute(HttpMethod::GET, "/", homeHandler);
    // Correct request and expected response
    HttpRequest request{HttpMethod::GET, "/", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{HttpStatus::OK, "Home page", {}};
    // The assertion
    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, PostExistingRouteReturnsOK) {
    Router router;
    router.addRoute(HttpMethod::GET, "/", homeHandler);
    router.addRoute(HttpMethod::POST, "/", postHelloHandler);
    
    HttpRequest request{HttpMethod::POST, "/", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{HttpStatus::OK, "Hello from POST", {}};
    
    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, GetNotFoundEndpointReturnsNotFound) {
    Router router;
    router.addRoute(HttpMethod::GET, "/", homeHandler);
    
    HttpRequest request{HttpMethod::GET, "/unknown", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{HttpStatus::NotFound, "Not Found", {}};

    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, DeleteExistingRouteReturnsMethodNotAllowed) {
    Router router;
    router.addRoute(HttpMethod::GET, "/", homeHandler);
    router.addRoute(HttpMethod::GET, "/hello", helloHandler);
    router.addRoute(HttpMethod::POST, "/hello", postHelloHandler);

    HttpRequest request{HttpMethod::DELETE, "/hello", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);

    std::string expectedString{"Method Not Allowed\nAllowed Methods: GET, POST\n"};
    Headers expectedHeaders;
    expectedHeaders.set("Allow", "GET, POST");
    HttpResponse expectedResponse{HttpStatus::MethodNotAllowed, expectedString, expectedHeaders};

    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}