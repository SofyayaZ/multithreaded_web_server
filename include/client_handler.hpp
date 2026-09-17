#pragma once

#include <socket.hpp>
#include <router.hpp>
#include <response_serializer.hpp>


void handleClient(const Socket& clientSocket,
                  const Router& router, 
                  const ResponseSerializer& serializer);