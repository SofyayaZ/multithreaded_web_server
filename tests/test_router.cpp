#include "app_interface/headers.hpp"
#include "http_method.hpp"
#include "http/http_response.hpp"
#include "routing/router.hpp"

#include <gtest/gtest.h>


HttpResponse homeHandler(const HttpRequest& request) {
    return {200, "Home page", {}};
}

HttpResponse helloHandler(const HttpRequest& request) {
    return {200, "Hello", {}};
}

HttpResponse postHelloHandler(const HttpRequest& request) {
    return {200, "Hello from POST", {}};
}

TEST(RouterTest, GetExistingRouteReturns200) {
    // Added router with GET route for test
    Router router;
    router.addRoute(HttpMethod::GET, "/", homeHandler);
    // Correct request and expected response
    HttpRequest request{HttpMethod::GET, "/", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{200, "Home page", {}};
    // The assertion
    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, PostExistingRouteReturns200) {
    Router router;
    router.addRoute(HttpMethod::GET, "/", homeHandler);
    router.addRoute(HttpMethod::POST, "/", postHelloHandler);
    
    HttpRequest request{HttpMethod::POST, "/", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{200, "Hello from POST", {}};
    
    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, GetNotFoundEndpointReturns404) {
    Router router;
    router.addRoute(HttpMethod::GET, "/", homeHandler);
    
    HttpRequest request{HttpMethod::GET, "/unknown", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{404, "Not Found", {}};

    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, DeleteExistingRouteReturns405) {
    Router router;
    router.addRoute(HttpMethod::GET, "/", homeHandler);
    router.addRoute(HttpMethod::GET, "/hello", helloHandler);
    router.addRoute(HttpMethod::POST, "/hello", postHelloHandler);

    HttpRequest request{HttpMethod::DELETE, "/hello", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);

    std::string expectedString{"Method Not Allowed\nAllowed Methods: GET, POST\n"};
    Headers expectedHeaders;
    expectedHeaders.set("Allow", "GET, POST");
    HttpResponse expectedResponse{405, expectedString, expectedHeaders};

    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}