#include "http_server.hpp"
#include <chrono>
#include <thread>


int main () {
    HttpServer server;
    server.run();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    server.stop();
    return 0;
}