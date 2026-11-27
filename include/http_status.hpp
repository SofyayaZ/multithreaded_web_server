#pragma once

#include <optional>
#include <string_view>


enum class HttpStatus : unsigned short {
    Continue                    = 100,
    SwitchingProtocols          = 101,

    OK                          = 200,
    Created                     = 201,
    Accepted                    = 202,
    NoContent                   = 204,
    PartialContent              = 206,

    MovedPermanently            = 301,
    Found                       = 302,
    NotModified                 = 304,
    TemporaryRedirect           = 307,
    PermanentRedirect           = 308,

    BadRequest                  = 400,
    Unauthorized                = 401,
    Forbidden                   = 403,
    NotFound                    = 404,
    MethodNotAllowed            = 405,
    NotAcceptable               = 406,
    RequestTimeout              = 408,
    Conflict                    = 409,
    Gone                        = 410,
    LengthRequired              = 411,
    PreconditionFailed          = 412,
    ContentTooLarge             = 413,
    URITooLong                  = 414,
    UnsupportedMediaType        = 415,
    RangeNotSatisfiable         = 416,
    ExpectationFailed           = 417,
    UnprocessableContent        = 422,
    UpgradeRequired             = 426,
    TooManyRequests             = 429,
    RequestHeaderFieldsTooLarge = 431,

    InternalServerError         = 500,
    NotImplemented              = 501,
    BadGateway                  = 502,
    ServiceUnavailable          = 503,
    GatewayTimeout              = 504,
    HTTPVersionNotSupported     = 505
};

constexpr unsigned codeFromStatus(HttpStatus httpStatus) noexcept {
    return static_cast<unsigned>(httpStatus);
}

constexpr std::string_view reasonPhrase(HttpStatus httpStatus) noexcept {
    switch (httpStatus) {
        case HttpStatus::Continue:                      return "Continue";
        case HttpStatus::SwitchingProtocols:            return "Switching Protocols";
        case HttpStatus::OK:                            return "OK";
        case HttpStatus::Created:                       return "Created";
        case HttpStatus::Accepted:                      return "Accepted";
        case HttpStatus::NoContent:                     return "No Content";
        case HttpStatus::PartialContent:                return "Partial Content";
        case HttpStatus::MovedPermanently:              return "Moved Permanently";
        case HttpStatus::Found:                         return "Found";
        case HttpStatus::NotModified:                   return "Not Modified";
        case HttpStatus::TemporaryRedirect:             return "Temporary Redirect";
        case HttpStatus::PermanentRedirect:             return "Permanent Redirect";
        case HttpStatus::BadRequest:                    return "Bad Request";
        case HttpStatus::Unauthorized:                  return "Unauthorized";
        case HttpStatus::Forbidden:                     return "Forbidden";
        case HttpStatus::NotFound:                      return "Not Found";
        case HttpStatus::MethodNotAllowed:              return "Method Not Allowed";
        case HttpStatus::NotAcceptable:                 return "Not Acceptable";
        case HttpStatus::RequestTimeout:                return "Request Timeout";
        case HttpStatus::Conflict:                      return "Conflict";
        case HttpStatus::Gone:                          return "Gone";
        case HttpStatus::LengthRequired:                return "Length Required";
        case HttpStatus::PreconditionFailed:            return "Precondition Failed";
        case HttpStatus::ContentTooLarge:               return "Content Too Large";
        case HttpStatus::URITooLong:                    return "URI Too Long";
        case HttpStatus::UnsupportedMediaType:          return "Unsupported Media Type";
        case HttpStatus::RangeNotSatisfiable:           return "Range Not Satisfiable";
        case HttpStatus::ExpectationFailed:             return "Expectation Failed";
        case HttpStatus::UnprocessableContent:          return "Unprocessable Content";
        case HttpStatus::UpgradeRequired:               return "Upgrade Required";
        case HttpStatus::TooManyRequests:               return "Too Many Requests";
        case HttpStatus::RequestHeaderFieldsTooLarge:   return "Request Header Fields Too Large";
        case HttpStatus::InternalServerError:           return "Internal Server Error";
        case HttpStatus::NotImplemented:                return "Not Implemented";
        case HttpStatus::BadGateway:                    return "Bad Gateway";
        case HttpStatus::ServiceUnavailable:            return "Service Unavailable";
        case HttpStatus::GatewayTimeout:                return "Gateway Timeout";
        case HttpStatus::HTTPVersionNotSupported:       return "HTTP Version Not Supported";
    }
    return "Unknown";
}

constexpr std::optional<HttpStatus> statusFromCode(unsigned code) noexcept {
    switch(code) {
        case(100) : return HttpStatus::Continue;
        case(101) : return HttpStatus::SwitchingProtocols;

        case(200) : return HttpStatus::OK;
        case(201) : return HttpStatus::Created;
        case(202) : return HttpStatus::Accepted;
        case(204) : return HttpStatus::NoContent;
        case(206) : return HttpStatus::PartialContent;

        case(301) : return HttpStatus::MovedPermanently;
        case(302) : return HttpStatus::Found;
        case(304) : return HttpStatus::NotModified;
        case(307) : return HttpStatus::TemporaryRedirect;
        case(308) : return HttpStatus::PermanentRedirect;

        case(400) : return HttpStatus::BadRequest;
        case(401) : return HttpStatus::Unauthorized;
        case(403) : return HttpStatus::Forbidden;
        case(404) : return HttpStatus::NotFound;
        case(405) : return HttpStatus::MethodNotAllowed;
        case(406) : return HttpStatus::NotAcceptable;
        case(408) : return HttpStatus::RequestTimeout;
        case(409) : return HttpStatus::Conflict;
        case(410) : return HttpStatus::Gone;
        case(411) : return HttpStatus::LengthRequired;
        case(412) : return HttpStatus::PreconditionFailed;
        case(413) : return HttpStatus::ContentTooLarge;
        case(414) : return HttpStatus::URITooLong;
        case(415) : return HttpStatus::UnsupportedMediaType;
        case(416) : return HttpStatus::RangeNotSatisfiable;
        case(417) : return HttpStatus::ExpectationFailed;
        case(422) : return HttpStatus::UnprocessableContent;
        case(426) : return HttpStatus::UpgradeRequired;
        case(429) : return HttpStatus::TooManyRequests;
        case(431) : return HttpStatus:: RequestHeaderFieldsTooLarge;

        case(500) : return HttpStatus::InternalServerError;
        case(501) : return HttpStatus::NotImplemented;
        case(502) : return HttpStatus::BadGateway;
        case(503) : return HttpStatus::ServiceUnavailable;
        case(504) : return HttpStatus::GatewayTimeout;
        case(505) : return HttpStatus::HTTPVersionNotSupported;
    }
    return std::nullopt;
}

constexpr std::string_view reasonPhrase(unsigned code) noexcept {
    const auto httpStatus = statusFromCode(code);
    return httpStatus.has_value() ? reasonPhrase(*httpStatus) : "Unknown";
}
