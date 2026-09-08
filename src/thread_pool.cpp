#include <thread_pool.hpp>


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
    {
        std::lock_guard<std::mutex> lock(mutex);
        stop = true;
    }
    // Notifying all workers about ThreadPool stopping
    condition.notify_all();
    for(auto& worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueue(int clientSocket) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (stop) {
            return;
        }
        clients.push(clientSocket);
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

        int clientSocket = clients.front();
        clients.pop();
        lock.unlock();

        handleClient(clientSocket, router, serializer);
    }
}