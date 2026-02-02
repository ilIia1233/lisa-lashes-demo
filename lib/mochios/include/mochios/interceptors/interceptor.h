#pragma once

#include <functional>

#include <mochios/messages/request.h>
#include <mochios/messages/response.h>

namespace mochios {

class Client;

namespace interceptor {

template <typename T> class Manager {
private:
  std::vector<std::function<void(T &)>> interceptors;

public:
  Manager() { return; }
  ~Manager() {
    this->interceptors.clear();
    return;
  }

  void use(std::function<void(T &)> interceptor) {
    interceptors.push_back(interceptor);
  }

  friend class mochios::Client;
};

typedef struct {
  mochios::interceptor::Manager<mochios::messages::Request> request;
  mochios::interceptor::Manager<mochios::messages::Response> response;
} Interceptors;

} // namespace interceptor

} // namespace mochios