#pragma once

#include <brewtils/string.h>
#include <brewtils/url.h>
#include <logger/log.h>

namespace mochios {

namespace messages {

class Cookie {
public:
  bool secure;
  bool httpOnly;
  bool partitioned;

  std::string name;
  std::string value;
  std::string domain;
  std::string path;
  std::string expires;
  std::string maxAge;
  std::string sameSite;

  Cookie();
  Cookie(const std::string &data);
  ~Cookie();

  std::string serialize() const;
};

} // namespace messages

} // namespace mochios