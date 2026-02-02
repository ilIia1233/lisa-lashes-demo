#include <brewtils/string.h>

std::string
brewtils::string::join(const std::vector<std::string> &strings,
                       const std::string &delimiter) noexcept(true) {
  std::string result;
  for (size_t i = 0; i < strings.size(); ++i) {
    if (i > 0) {
      result += delimiter;
    }
    result += strings[i];
  }
  return result;
}

std::string
brewtils::string::join(const std::set<std::string> &strings,
                       const std::string &delimiter) noexcept(true) {
  std::string result;
  std::set<std::string>::iterator it = strings.begin();
  result += *it;
  ++it;

  for (; it != strings.end(); ++it) {
    result += delimiter;
    result += *it;
  }

  return result;
}

std::vector<std::string>
brewtils::string::split(const std::string &str,
                        const std::string &delimiter) noexcept(true) {
  std::vector<std::string> result;
  size_t start = 0;
  size_t end = str.find(delimiter);
  result.reserve(str.size() / (delimiter.size() + 1));

  while (end != std::string::npos) {
    result.emplace_back(str.substr(start, end - start));
    start = end + delimiter.size();
    end = str.find(delimiter, start);
  }

  result.emplace_back(str.substr(start));
  return result;
}

std::string brewtils::string::trim(const std::string &str) noexcept(true) {
  std::regex pattern(R"(^\s+|\s+$)");
  return std::regex_replace(str, pattern, "");
}

std::string brewtils::string::replace(const std::string &str,
                                      const std::string &from,
                                      const std::string &to) noexcept(true) {
  std::string result = str;
  size_t start_pos = 0;
  while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
    result.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return result;
}

std::string brewtils::string::lower(const std::string &str) noexcept(true) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

std::string brewtils::string::upper(const std::string &str) noexcept(true) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::toupper);
  return result;
}
