#include <string>
#include <unordered_map>
#include <headers.hpp>


enum class HttpStatus {
    OK = 200,
    Created = 201,
    NoContent = 204,

    BadRequest = 400,
    Unauthorized = 401,
    NotFound = 404,
    MethodNotAllowed = 405,
    RequestTimeout = 408,
    RequestEntityTooLarge = 413,

    InternalServerError = 500
};

// struct for forming response to app (client -> server -> app)
struct ApplicationResponse {
    HttpStatus status;
    Headers headers{};
    std::string body{};
};