#include <router.hpp>


void Router::addRoute(std::string method, std::string endpoint, std::function<HttpResponse(const HttpRequest&)> handler) {
    routes.emplace_back(method, endpoint, handler);
};

HttpResponse Router::route(const HttpRequest& request) {
    HttpResponse response;
    std::string allowedMethods{};
    bool validEndpoint = false;
    bool firstMethod = true;
    for (const auto& route : routes) {
        if (route.endpoint == request.endpoint) {
            validEndpoint = true;
            if (route.method == request.httpMethod) {
                response = route.handler(request);
                return response;
            }
            if (firstMethod) {
                firstMethod = false;
                allowedMethods.append(route.method);
                continue;
            }
            allowedMethods.append(", ");
            allowedMethods.append(route.method);
        }
    }
    if (validEndpoint) {
        response.statusCode = 405;
        response.body = "Method Not Allowed";
        response.headers["Allow"] = allowedMethods;
    } else {
        response.statusCode = 404;
        response.body = "Not Found";
    }
    
    return response;
};