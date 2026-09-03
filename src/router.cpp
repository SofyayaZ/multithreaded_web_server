#include <router.hpp>
#include <route.hpp>
#include <functional>


void Router::addRoute(std::string method, std::string endpoint, std::function<HttpResponse(const HttpRequest&)> handler) {
    Route newRoute = Route(method, endpoint, handler);
    routes.push_back(newRoute);
};

HttpResponse Router::route(const HttpRequest& request) {
    HttpResponse response = HttpResponse();
    response.header = request.httpVersion;
    if (request.httpMethod == "GET") {
        if (request.endpoint == "/") {
            response.statusCode = 200;
            response.body = "Home page";
            
        }
        else if (request.endpoint == "/hello") {
            response.statusCode = 200;
            response.body = "Hello";
        }
        else {
            response.statusCode = 404;
            response.body = "The page does not exist";
        }
    } else {
        response.statusCode = 405;
        response.body = "Invalid method";
    }
    response.header += std::to_string(response.statusCode) + "\r\n";
    response.header += "Content-Length: " + std::to_string(static_cast<int>(response.body.size())) + "\r\n";
    response.header += "Content-type: text/plain\r\n";
    response.header += "Connection: close\r\n";
    response.header += "\r\n";
    response.header += response.body;
    return response;
};