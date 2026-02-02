#include <brewtils/base64.h>

namespace brewtils {

namespace base64 {

static constexpr const char *BASE64_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

} // namespace base64

} // namespace brewtils

std::string brewtils::base64::encode(const std::string &data) noexcept(true) {
  std::string encoded;
  size_t len = data.size();
  size_t i = 0;
  while (i < len) {
    encoded += brewtils::base64::BASE64_CHARS[(data[i] >> 2) & 0x3F];
    if (i + 1 < len) {
      encoded += brewtils::base64::BASE64_CHARS[((data[i] & 0x3) << 4) |
                                                ((data[i + 1] >> 4) & 0xF)];
      if (i + 2 < len) {
        encoded += brewtils::base64::BASE64_CHARS[((data[i + 1] & 0xF) << 2) |
                                                  ((data[i + 2] >> 6) & 0x3)];
        encoded += brewtils::base64::BASE64_CHARS[data[i + 2] & 0x3F];
      } else {
        encoded += brewtils::base64::BASE64_CHARS[(data[i + 1] & 0xF) << 2];
        encoded += '=';
      }
    } else {
      encoded += brewtils::base64::BASE64_CHARS[(data[i] & 0x3) << 4];
      encoded += "==";
    }
    i += 3;
  }

  return encoded;
}

std::string brewtils::base64::decode(const std::string &data) noexcept(false) {
  size_t len = data.size();
  if (len % 4 != 0) {
    logger::error("Input is not valid Base64 encoding: " + data,
                  "std::string brewtils::base64::decode(const std::string "
                  "&data) noexcept(false)");
  }

  size_t padding = 0;
  if (len > 0 && data[len - 1] == '=')
    padding++;
  if (len > 1 && data[len - 2] == '=')
    padding++;

  size_t decoded_size = len / 4 * 3 - padding;
  std::string decoded;
  decoded.reserve(decoded_size);

  int num = 0;
  int countBits = 0;
  for (size_t i = 0; i < len; i += 4) {
    num = 0, countBits = 0;
    for (int j = 0; j < 4; j++) {
      num = num << 6;
      char currentChar = data[i + j];
      if (currentChar == '=')
        continue;
      const char *pos =
          std::strchr(brewtils::base64::BASE64_CHARS, currentChar);
      if (pos == nullptr)
        logger::error("Invalid Base64 character: " +
                          std::string(1, currentChar),
                      "std::string brewtils::base64::decode(const std::string "
                      "&data) noexcept(false)");
      num |= static_cast<int>(pos - brewtils::base64::BASE64_CHARS);
      countBits += 6;
    }

    if (data[i + 2] == '=')
      countBits -= 4;
    else if (data[i + 3] == '=')
      countBits -= 2;

    while (countBits >= 8) {
      countBits -= 8;
      decoded.push_back(static_cast<char>((num >> countBits) & 255));
    }
  }

  return decoded;
}

bool brewtils::base64::isValid(const std::string &data) noexcept(true) {
  size_t len = data.size();
  if (len % 4 != 0)
    return false;

  size_t padding = 0;
  if (len > 0 && data[len - 1] == '=')
    padding++;
  if (len > 1 && data[len - 2] == '=')
    padding++;

  if (len / 4 * 3 - padding == 0)
    return false;

  for (char c : data) {
    if (!std::isalnum(c) && c != '+' && c != '/' && c != '=')
      return false;
  }

  return true;
}
