#include <router.hpp>


void Router::addRoute(std::string method, std::string endpoint, std::function<HttpResponse(const HttpRequest&)> handler) {
    Route newRoute = Route(method, endpoint, handler);
    routes.push_back(newRoute);
};

HttpResponse Router::route(const HttpRequest& request) {
    HttpResponse response;
    bool validMethod = false;
    for (const auto& route : routes) {
        if (route.method == request.httpMethod) {
            validMethod = true;
            if (route.endpoint == request.endpoint) {
                response = route.handler(request);
                return response;
            }
        }
    }
    if (validMethod) {
        response.statusCode = 404;
        response.body = "This endpoint does not exist";
    }
    else {
        response.statusCode = 405;
        response.body = "Invalid method";
    }
    return response;
};