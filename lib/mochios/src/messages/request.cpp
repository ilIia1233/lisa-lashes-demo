#include <mochios/messages/request.h>

mochios::messages::Request::Request(const std::string &path) : path(path) {
  return;
}

mochios::messages::Request::~Request() { return; }

const void mochios::messages::Request::print() const {
  logger::debug("Request:");
  logger::debug("  path: " + this->path);
  logger::debug("  method: " + this->method);
  logger::debug("  headers:");
  for (const std::pair<std::string, std::string> &header : this->headers) {
    logger::debug("    " + header.first + ": " + header.second);
  }

  logger::debug("  params:");
  for (const std::pair<std::string, std::string> &param : this->params) {
    logger::debug("    " + param.first + ": " + param.second);
  }

  logger::debug("  queries:");
  for (const std::pair<std::string, std::string> &query : this->queries) {
    logger::debug("    " + query.first + ": " + query.second);
  }

  logger::debug("  cookies:");
  for (mochios::messages::Cookie *cookie : this->cookies) {
    logger::debug("    " + cookie->serialize());
  }

  logger::debug("  body:");
  logger::debug("    " + this->body.dumps(2, 2));
  return;
}