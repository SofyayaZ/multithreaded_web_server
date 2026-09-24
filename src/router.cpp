#include <router.hpp>
#include <iostream>


void Router::addRoute(std::string method, std::string endpoint, std::function<HttpResponse(const HttpRequest&)> handler) {
    routes.emplace_back(method, endpoint, handler);
};

HttpResponse Router::route(const HttpRequest& request) const {
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
        response.headers["Allow"] = allowedMethods;
        response.body = "Method Not Allowed\n";
        response.body += "Allowed Methods" + allowedMethods + "\n";
    } else {
        response.statusCode = 404;
        response.body = "Not Found";
    }
    
    return response;
};