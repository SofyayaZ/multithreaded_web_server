#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>


bool sendAll(int clientSocket, const std::string& response);
