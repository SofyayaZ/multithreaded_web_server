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
#include <thread>
#include <csignal>

#include <http_request.hpp>
#include <http_response.hpp>
#include <router.hpp>
#include <response_handlers.hpp>
#include <request_parser.hpp>
#include <response_serializer.hpp>
#include <client_handler.hpp>
#include <thread_pool.hpp>
#include <socket.hpp>


volatile sig_atomic_t stop = 0;
void handleSignal(int signal);
int main();