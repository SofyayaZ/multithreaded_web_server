#pragma once

#include <chrono>
#include <cstddef>

// Describes server configuration
struct ServerConfig {
    int port = 8080;
    std::size_t workersCount = 4;
    std::size_t maxConnections = 4;
    std::size_t maxHeaderBytes = 8192;
    std::size_t maxBodyBytes = 8192;
    std::size_t maxPendingClients = 8;
    std::chrono::seconds headerTimeout{15};
    std::chrono::seconds bodyTimeout{60};
    std::chrono::seconds idleTimeout{30};
};