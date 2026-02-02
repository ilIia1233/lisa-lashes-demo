#include <brewtils/url.h>

namespace brewtils {

namespace url {

static constexpr const char *HEX_DIGITS = "0123456789ABCDEF";

} // namespace url

} // namespace brewtils

std::string brewtils::url::encode(const std::string &data) noexcept(true) {
  std::string result;
  result.reserve(data.size() << 1);
  for (char ch : data) {
    if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= 'a' && ch <= 'z') || ch == '-' || ch == '_' || ch == '!' ||
        ch == '\'' || ch == '(' || ch == ')' || ch == '*' || ch == '~' ||
        ch == '.') {
      result.push_back(ch);
    } else {
      result.push_back('%');
      result.push_back(
          brewtils::url::HEX_DIGITS[static_cast<unsigned char>(ch) >> 4]);
      result.push_back(
          brewtils::url::HEX_DIGITS[static_cast<unsigned char>(ch) & 15]);
    }
  }

  return result;
}

std::string brewtils::url::decode(const std::string &data) noexcept(true) {
  std::string result;
  result.reserve(data.size());
  for (size_t i = 0; i < data.size(); i++) {
    char ch = data[i];
    if (ch == '%' && (i + 2) < data.size()) {
      std::string hex = data.substr(i + 1, 2);
      char dec = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
      result.push_back(dec);
      i += 2;
    } else if (ch == '+') {
      result.push_back(' ');
    } else {
      result.push_back(ch);
    }
  }

  return result;
}
