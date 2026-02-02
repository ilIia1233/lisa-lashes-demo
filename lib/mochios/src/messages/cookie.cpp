#include <mochios/helpers/cookie.h>
#include <mochios/messages/cookie.h>

mochios::messages::Cookie::Cookie()
    : secure(false), httpOnly(false), partitioned(false), name(""), value(""),
      domain(""), path(""), expires(""), maxAge(""), sameSite("") {
  return;
}

mochios::messages::Cookie::Cookie(const std::string &data) {
  std::string key, value;
  std::vector<std::string> kv;
  std::vector<std::string> parts = brewtils::string::split(data, ";");

  for (const std::string &part : parts) {
    kv = brewtils::string::split(part, "=");
    if (kv.size() == 0) {
      continue;
    }

    key = brewtils::string::lower(
        brewtils::url::decode(brewtils::string::trim(kv[0])));
    if (kv.size() == 1) {
      if (key == "secure") {
        this->secure = true;
      } else if (key == "httponly") {
        this->httpOnly = true;
      } else if (key == "partitioned") {
        this->partitioned = true;
      }
      continue;
    }

    value = brewtils::url::decode(brewtils::string::trim(kv[1]));
    if (key == "domain") {
      this->domain = value;
    } else if (key == "path") {
      this->path = value;
    } else if (key == "expires") {
      this->expires = value;
    } else if (key == "max-age") {
      this->maxAge = value;
    } else if (key == "samesite") {
      this->sameSite = value;
    } else {
      this->name = brewtils::url::decode(brewtils::string::trim(kv[0]));
      this->value = value;
    }
  }

  if (this->name.empty()) {
    logger::warning("Cookie name empty in data: " + data);
  }

  return;
}

mochios::messages::Cookie::~Cookie() { return; }

std::string mochios::messages::Cookie::serialize() const {
  return mochios::helpers::cookie::serialize(*this);
}