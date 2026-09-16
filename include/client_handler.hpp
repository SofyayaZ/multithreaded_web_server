#pragma once
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <request_parser.hpp>
#include <http_response.hpp>
#include <router.hpp>
#include <response_serializer.hpp>
#include <send_all.hpp>
#include <socket.hpp>


void handleClient(const Socket& clientSocket,
                  const Router& router, 
                  const ResponseSerializer& serializer);