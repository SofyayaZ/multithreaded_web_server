#pragma once

#include "app_interface/server_config.hpp"
#include "http/response_serializer.hpp"
#include "socket.hpp"
#include "routing/router.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


class ThreadPool {
public:
    ThreadPool(const ServerConfig& config, Router& router, ResponseSerializer& serializer);
    ~ThreadPool();
    void enqueue(Socket client);
private:
    void worker();
    // Stop accepting new tasks and shut down workers after the queue is drained
    bool stop = false;
    std::vector<std::thread> workers;
    std::queue<Socket> clients;
    std::mutex mutex;
    std::condition_variable condition;

    const ServerConfig& config;
    Router& router;
    ResponseSerializer& serializer;
};
