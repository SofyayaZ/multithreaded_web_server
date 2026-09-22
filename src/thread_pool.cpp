#include <thread_pool.hpp>

#include <iostream>
#include <cstddef>
#include <client_handler.hpp>


constexpr size_t MAX_PENDING_CLIENTS = 8;

ThreadPool::ThreadPool(size_t countWorkers,
                       Router& router,
                       ResponseSerializer& serializer):
    router(router),
    serializer(serializer) {
    for (size_t i = 0; i < countWorkers; ++i) {
        workers.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() {
    std::cout << "ThreadPool destructor is called\n";
    {
        std::lock_guard<std::mutex> lock(mutex);
        stop = true;
    }
    // Notifying all workers about ThreadPool stopping
    condition.notify_all();
    for(auto& worker : workers) {
        worker.join();
    }
    std::cout << "All workers has been joined\n";
}

void ThreadPool::enqueue(Socket clientSocket) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (stop) {
            return;
        }
        if (clients.size() < MAX_PENDING_CLIENTS) {
            clients.push(std::move(clientSocket));
        }
        else {
            return;
        }
    }
    condition.notify_one();
}

void ThreadPool::worker() {
    while(true) {
        std::unique_lock<std::mutex> lock(mutex);

        condition.wait(lock, [this] {
            return stop || !clients.empty();
        });

        // If the clients queue is not empty, but the pool has already stoped
        if (stop && clients.empty()) {
            return;
        }

        Socket clientSocket = std::move(clients.front());
        clients.pop();
        lock.unlock();

        handleClient(clientSocket, router, serializer);
    }
}