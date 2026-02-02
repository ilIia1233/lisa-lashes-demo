#pragma once

#include <functional>
#include <thread>
#include <utility>

#include <logger/log.h>

namespace nexus {

template <class F, class... Args>
void async(F &&f, Args &&...args) noexcept(false) {
  try {
    std::thread(std::forward<F>(f), std::forward<Args>(args)...).detach();
  } catch (const std::exception &e) {
    logger::error(
        e.what(),
        "template <class F, class... Args> void async(F &&f, Args &&...args)");
  }
}

} // namespace nexus