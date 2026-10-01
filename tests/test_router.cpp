#include <gtest/gtest.h>
#include <router.hpp>
#include <http_response.hpp>


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
    router.addRoute("GET", "/", homeHandler);
    // Correct request and expected response
    HttpRequest request{"GET", "/", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{200, "Home page", {}};
    // The assertion
    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, PostExistingRouteReturns200) {
    Router router;
    router.addRoute("GET", "/", homeHandler);
    router.addRoute("POST", "/", postHelloHandler);
    
    HttpRequest request{"POST", "/", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{200, "Hello from POST", {}};
    
    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, GetNotFoundEndpointReturns404) {
    Router router;
    router.addRoute("GET", "/", homeHandler);
    
    HttpRequest request{"GET", "/unknown", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{404, "Not Found", {}};

    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}

TEST(RouterTest, DeleteExistingRouteReturns405) {
    Router router;
    router.addRoute("GET", "/", homeHandler);
    router.addRoute("GET", "/hello", helloHandler);
    router.addRoute("POST", "/hello", postHelloHandler);

    HttpRequest request{"DELETE", "/hello", "HTTP/1.1"};
    HttpResponse realResponse = router.route(request);
    HttpResponse expectedResponse{405, "Method Not Allowed\nAllowed Methods: GET, POST\n", {{"Allow", "GET, POST"}}};

    EXPECT_EQ(realResponse.statusCode, expectedResponse.statusCode);
    EXPECT_EQ(realResponse.body, expectedResponse.body);
    EXPECT_EQ(realResponse.headers, expectedResponse.headers);
}