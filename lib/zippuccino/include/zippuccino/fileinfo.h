#pragma once

#include <cstdint>
#include <string>

namespace zippuccino {

typedef struct {
  std::string name;
  uint32_t size;
  uint32_t offset;
  uint32_t crc32;
} FileInfo;

} // namespace zippuccino