#pragma once

#include <mochios/messages/cookie.h>

namespace mochios {

namespace helpers {

namespace cookie {

std::string serialize(const mochios::messages::Cookie &cookie);

} // namespace cookie

} // namespace helpers

} // namespace mochios