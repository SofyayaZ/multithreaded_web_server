#pragma once

#include "socket.hpp"
#include "routing/router.hpp"
#include "http/response_serializer.hpp"
#include "app_interface/server_config.hpp"


void handleClient(const Socket& clientSocket,
                  const Router& router, 
                  const ResponseSerializer& serializer,
                  const ServerConfig& config);