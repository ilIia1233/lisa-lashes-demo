#pragma once

#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

namespace mochios {

namespace enums {

enum class method { GET, POST, PUT, PATCH, DELETE, OPTIONS, HEAD };

extern const std::set<std::string> methods;

} // namespace enums

} // namespace mochios

bool operator==(const mochios::enums::method &m, const std::string &str);
bool operator!=(const mochios::enums::method &m, const std::string &str);

std::ostream &operator<<(std::ostream &os, const mochios::enums::method &m);

std::string operator+(const mochios::enums::method &m, const std::string &str);
std::string operator+(const std::string &str, const mochios::enums::method &m);
