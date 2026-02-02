#pragma once

#include <mochios/messages/message.h>

namespace mochios {

namespace messages {

class Response : public Message {
private:
public:
  Response();
  ~Response();

  int statusCode;
  std::string statusText;

  const void print() const override;
};

} // namespace messages

} // namespace mochios