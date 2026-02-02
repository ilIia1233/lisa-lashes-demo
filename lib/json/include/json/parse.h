#pragma once

#include <cctype>
#include <sstream>

#include <logger/log.h>

#include <json/object.h>

namespace json {

class parser {
private:
  std::string input;
  object output;

  size_t index;

  object parseValue() noexcept(false);
  object parseBooleanNullOrUndefined() noexcept(false);
  object parseNumber() noexcept(true);
  object parseString() noexcept(true);
  object parseArray() noexcept(false);
  object parseMap() noexcept(false);

  void skipWhitespace() noexcept(true);
  bool shouldStringBeEscaped() noexcept(true);
  char getPrevChar() noexcept(true);
  char getCurrChar() noexcept(true);

public:
  parser() noexcept(true);
  parser(std::string data) noexcept(false);
  ~parser() noexcept(true);

  object load(std::string path) noexcept(false);
  object loads(std::string data) noexcept(false);
};

} // namespace json