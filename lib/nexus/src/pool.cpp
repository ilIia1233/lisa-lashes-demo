#include <nexus/pool.h>

nexus::pool::pool(size_t numThreads) : isRunning(true) {
  logger::success("Running nexus with " + std::to_string(numThreads) +
                  " threads");
  for (size_t i = 0; i < numThreads; i++) {
    this->workers.emplace_back([this] {
      while (true) {
        std::function<void()> task;

        {
          std::unique_lock<std::mutex> lock(this->mutex);
          this->condition.wait(lock, [this] {
            return !this->isRunning || !this->tasks.empty();
          });

          if (!this->isRunning && this->tasks.empty()) {
            return;
          }

          task = std::move(this->tasks.front());
          this->tasks.pop();
        }

        task();
      }
    });
  }
}

nexus::pool::~pool() {
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->isRunning = false;
  }

  this->condition.notify_all();
  for (std::thread &worker : this->workers) {
    worker.join();
  }
}
