#pragma once

#include <chrono>
#include <cstddef>
#include <sys/socket.h>

// Describes server configuration
struct ServerConfig {
    int port = 8080;                        // default port for server connections
    int listenBacklog = SOMAXCONN;          // max accept queue size
    std::size_t workersCount = 4;           // number of workers
    std::size_t maxConnections = 4;         
    std::size_t maxHeaderBytes = 8192;
    std::size_t maxBodyBytes = 8192;        
    std::size_t maxPendingClients = 8;      // max clients waiting in queue
    std::chrono::seconds headerTimeout{15};
    std::chrono::seconds bodyTimeout{60};
    std::chrono::seconds idleTimeout{30};
};