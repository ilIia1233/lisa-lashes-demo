#pragma once

#include <brewtils/string.h>
#include <json/parse.h>

#include <mochios/messages/cookie.h>

namespace mochios {

namespace messages {

class Message {
private:
protected:
public:
  Message();
  ~Message();

  // Don't use headers directly! Use them using the get and set
  // methods
  std::map<std::string, std::string> headers;
  std::map<std::string, std::string> params;
  std::map<std::string, std::string> queries;
  std::vector<mochios::messages::Cookie *> cookies;
  json::object body;

  void set(const std::string &key, const std::string &value);
  const std::string get(const std::string &key) const;

  virtual const void print() const = 0;
};

} // namespace messages

} // namespace mochios