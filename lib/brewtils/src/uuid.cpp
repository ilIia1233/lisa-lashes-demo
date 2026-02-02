#include <brewtils/uuid.h>

namespace brewtils {

namespace uuid {

static constexpr const char HEX_CHARS[] = "0123456789abcdef";

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);

} // namespace uuid

} // namespace brewtils

std::string brewtils::uuid::v4() noexcept(true) {
  std::string uuid;
  uuid.resize(36);

  for (int i = 0; i < 36; ++i) {
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      uuid += '-';
    } else if (i == 14) {
      uuid += '4';
    } else if (i == 19) {
      uuid += brewtils::uuid::HEX_CHARS
          [(brewtils::uuid::dis(brewtils::uuid::gen) & 0x3) | 0x8];
    } else {
      uuid +=
          brewtils::uuid::HEX_CHARS[brewtils::uuid::dis(brewtils::uuid::gen)];
    }
  }

  return uuid;
}

std::string brewtils::uuid::v5(const std::string &namespaceUUID,
                               const std::string &name) noexcept(true) {
  std::array<unsigned char, 16> namespaceBytes;
  for (int i = 0; i < 16; ++i) {
    namespaceBytes[i] = static_cast<unsigned char>(
        std::strtoul(namespaceUUID.substr(2 * i, 2).c_str(), nullptr, 16));
  }

  std::array<unsigned char, 20> hash;
  std::hash<std::string> hasher;
  std::string combined = namespaceUUID + name;
  std::size_t combined_hash = hasher(combined);
  std::copy(reinterpret_cast<unsigned char *>(&combined_hash),
            reinterpret_cast<unsigned char *>(&combined_hash) +
                sizeof(combined_hash),
            hash.begin());
  hash[6] = (hash[6] & 0x0F) | 0x50;
  hash[8] = (hash[8] & 0x3F) | 0x80;

  std::stringstream uuid;
  uuid << std::hex << std::setfill('0');
  for (int i = 0; i < 16; ++i) {
    uuid << std::setw(2) << static_cast<unsigned int>(hash[i]);
    if (i == 3 || i == 5 || i == 7 || i == 9) {
      uuid << '-';
    }
  }
  return uuid.str();
}
