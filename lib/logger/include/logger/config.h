#pragma once

#include <string>

namespace logger {

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;36m"
#define MAGENTA "\033[1;35m"
#define RESET "\033[0m"

extern bool ENABLED;
extern bool BEAUTIFY;
extern bool SHOW_TIME;

extern std::string TIME_FORMAT;

} // namespace logger