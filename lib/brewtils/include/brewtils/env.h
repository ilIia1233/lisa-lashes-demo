#pragma once

#include <brewtils/os.h>
#include <brewtils/string.h>

namespace brewtils {

namespace env {

void init(std::string path = brewtils::os::dir::current()) noexcept(false);

std::string get(const std::string &key,
                const std::string &defaultValue = "") noexcept(true);

void set(const std::string &key, const std::string &value) noexcept(false);

} // namespace env

} // namespace brewtils
