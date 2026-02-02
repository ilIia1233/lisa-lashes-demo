#include <mochios/messages/response.h>

mochios::messages::Response::Response() { return; }

mochios::messages::Response::~Response() { return; }

const void mochios::messages::Response::print() const {
  logger::debug("Response:");
  logger::debug("  statusCode: " + std::to_string(this->statusCode));
  logger::debug("  statusText: " + this->statusText);

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