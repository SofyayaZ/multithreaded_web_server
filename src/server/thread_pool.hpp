#pragma once

#include "routing/router.hpp"
#include "http/response_serializer.hpp"
#include "socket.hpp"
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>


class ThreadPool {
public:
    ThreadPool(size_t countWorkers, Router& router, ResponseSerializer& serializer);
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

    Router& router;
    ResponseSerializer& serializer;
};