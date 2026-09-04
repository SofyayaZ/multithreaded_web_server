#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <optional>
#include <functional>

#include <http_request.hpp>
#include <http_response.hpp>
#include <router.hpp>
#include <handlers.hpp>
#include <request_parser.hpp>
#include <response_serializer.hpp>

HttpResponse homeHandler(const HttpRequest& request);
HttpResponse helloHandler(const HttpRequest& request);
int main();