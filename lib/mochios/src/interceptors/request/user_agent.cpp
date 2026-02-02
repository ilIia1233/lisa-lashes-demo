#include <mochios/interceptors/request/user_agent.h>

const std::string mochiosUserAgent = "mochios/" + mochios::version;

void mochios::interceptor::request::userAgent(
    mochios::messages::Request &request) {
  request.set("User-Agent", mochiosUserAgent);
}