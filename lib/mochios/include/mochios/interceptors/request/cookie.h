#pragma once

#include <mochios/messages/cookie.h>
#include <mochios/messages/request.h>

namespace mochios {

namespace interceptor {

namespace request {

void cookie(mochios::messages::Request &request);

} // namespace request

} // namespace interceptor

} // namespace mochios