#include <mochios/messages/message.h>

mochios::messages::Message::Message() { return; }

mochios::messages::Message::~Message() {
  for (mochios::messages::Cookie *cookie : this->cookies) {
    delete cookie;
  }

  return;
}

void mochios::messages::Message::set(const std::string &key,
                                     const std::string &value) {
  this->headers[brewtils::string::lower(key)] = value;
  return;
}

const std::string
mochios::messages::Message::get(const std::string &key) const {
  return this->headers.at(brewtils::string::lower(key));
}
