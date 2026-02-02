#include <mochios/interceptors/request/cookie.h>

void mochios::interceptor::request::cookie(
    mochios::messages::Request &request) {
  bool isFirst = true;
  std::string value = "";
  for (const mochios::messages::Cookie *cookie : request.cookies) {
    if (isFirst) {
      isFirst = false;
    } else {
      value += "; ";
    }
    value += cookie->name + "=" + cookie->value;
  }

  if (!value.empty()) {
    request.headers["Cookie"] = value;
  }
  return;
}