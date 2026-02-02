#include <mochios/interceptors/response/json.h>

void mochios::interceptor::response::json(
    mochios::messages::Response &response) {
  if (response.get("content-type") != "application/json") {
    return;
  }

  json::parser parser;
  response.body = parser.loads(static_cast<std::string>(response.body));
}