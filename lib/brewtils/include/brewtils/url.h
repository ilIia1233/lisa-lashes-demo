#pragma once

#include <string>

namespace brewtils {

namespace url {

std::string encode(const std::string &data) noexcept(true);

std::string decode(const std::string &data) noexcept(true);

} // namespace url

} // namespace brewtils
