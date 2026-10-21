#include "http_server.hpp"
#include <thread>
#include <chrono>


int main () {
    HttpServer server;
    server.run(8080);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    server.stop();
    return 0;
}