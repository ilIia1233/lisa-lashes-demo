#pragma once

#include <array>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>

namespace brewtils {

namespace uuid {

std::string v4() noexcept(true);

std::string v5(const std::string &namespaceUUID = v4(),
               const std::string &name = "brewtils") noexcept(true);

} // namespace uuid

} // namespace brewtils