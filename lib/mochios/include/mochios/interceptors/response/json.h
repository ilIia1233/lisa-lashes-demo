#pragma once

#include <json/parse.h>

#include <mochios/messages/response.h>

namespace mochios {

namespace interceptor {

namespace response {

void json(mochios::messages::Response &response);

} // namespace response

} // namespace interceptor

} // namespace mochios