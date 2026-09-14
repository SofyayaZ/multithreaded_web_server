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


void handleClient(int clientSocket, Router& router, ResponseSerializer& serializer);