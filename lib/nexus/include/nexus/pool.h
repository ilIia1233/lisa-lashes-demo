#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <logger/log.h>

namespace nexus {

class pool {
private:
  bool isRunning;
  std::mutex mutex;
  std::condition_variable condition;
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

public:
  pool(size_t numThreads = std::thread::hardware_concurrency());
  ~pool();

  template <class F, class... Args>
  std::future<typename std::invoke_result<F, Args...>::type>
  enqueue(F &&f, Args &&...args) noexcept(false) {
    using return_type = typename std::invoke_result<F, Args...>::type;
    std::shared_ptr<std::packaged_task<return_type()>> task =
        std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> result = task->get_future();
    {
      std::unique_lock<std::mutex> lock(mutex);
      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return result;
  }
};

} // namespace nexus