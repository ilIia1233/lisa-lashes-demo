#pragma once

#include <mochios/messages/request.h>
#include <mochios/version.h>

namespace mochios {

namespace interceptor {

namespace request {

void userAgent(mochios::messages::Request &request);

} // namespace request

} // namespace interceptor

} // namespace mochios