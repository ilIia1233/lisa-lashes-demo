#include <zippuccino/crc.h>

bool zippuccino::crc::initialized = false;

uint32_t zippuccino::crc::table[256];

void zippuccino::crc::init() noexcept(true) {
  if (zippuccino::crc::initialized) {
    return;
  }

  zippuccino::crc::initialized = true;
  for (uint32_t i = 0; i < 256; i++) {
    uint32_t crc = i;
    for (size_t j = 0; j < 8; ++j) {
      if (crc & 1) {
        crc = (crc >> 1) ^ zippuccino::config::POLYNOMIAL;
      } else {
        crc >>= 1;
      }
    }
    zippuccino::crc::table[i] = crc;
  }
}

uint32_t zippuccino::crc::compute(const std::string &path) noexcept(false) {
  if (!zippuccino::crc::initialized) {
    zippuccino::crc::init();
  }

  if (!brewtils::os::file::exists(path)) {
    logger::error("File does not exist: " + path,
                  "uint32_t zippuccino::crc::compute(const std::string &path)");
  }

  uint32_t crc32 = 0xFFFFFFFF;
  std::ifstream file(path, std::ios::binary);
  std::vector<char> buffer(zippuccino::crc::bufferSize);

  try {
    while (file.read(buffer.data(), zippuccino::crc::bufferSize) ||
           file.gcount() > 0) {
      std::streamsize bytesRead = file.gcount();
      for (std::streamsize i = 0; i < bytesRead; i++) {
        crc32 =
            (crc32 >> 8) ^
            zippuccino::crc::table[(crc32 ^ static_cast<uint8_t>(buffer[i])) &
                                   0xFF];
      }
    }
    crc32 ^= 0xFFFFFFFF;
  } catch (const std::exception &e) {
    logger::error("Error reading file: " + path + " - " + std::string(e.what()),
                  "uint32_t zippuccino::crc::compute(const std::string &path)");
  } catch (...) {
    logger::error("Error reading file: " + path,
                  "uint32_t zippuccino::crc::compute(const std::string &path)");
  }

  file.close();
  return crc32;
}