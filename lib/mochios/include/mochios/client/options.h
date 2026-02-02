#pragma once

#include <string>

namespace mochios {

namespace client {

typedef struct {
  std::string host;
  unsigned short port;
  unsigned int timeout = 2;
} Connection;

} // namespace client

} // namespace mochios