#include <mochios/enums/method.h>

const std::set<std::string> mochios::enums::methods = {
    "GET", "POST", "PUT", "PATCH", "DELETE", "OPTIONS", "HEAD"};

static const std::unordered_map<mochios::enums::method, std::string>
    methodToString{{mochios::enums::method::GET, "GET"},
                   {mochios::enums::method::POST, "POST"},
                   {mochios::enums::method::PUT, "PUT"},
                   {mochios::enums::method::PATCH, "PATCH"},
                   {mochios::enums::method::DELETE, "DELETE"},
                   {mochios::enums::method::OPTIONS, "OPTIONS"},
                   {mochios::enums::method::HEAD, "HEAD"}};

bool operator==(const mochios::enums::method &m, const std::string &str) {
  return methodToString.at(m) == str;
}

bool operator!=(const mochios::enums::method &m, const std::string &str) {
  return !(m == str);
}

std::ostream &operator<<(std::ostream &os, const mochios::enums::method &m) {
  os << methodToString.at(m);
  return os;
}

std::string operator+(const mochios::enums::method &m, const std::string &str) {
  return methodToString.at(m) + str;
}

std::string operator+(const std::string &str, const mochios::enums::method &m) {
  return str + methodToString.at(m);
}
