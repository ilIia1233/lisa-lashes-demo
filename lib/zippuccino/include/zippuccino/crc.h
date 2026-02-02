#pragma once

#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>

#include <brewtils/os/file.h>

#include <zippuccino/config.h>

namespace zippuccino {

namespace crc {

extern bool initialized;

static const size_t bufferSize = 4096;

extern uint32_t table[256];

void init() noexcept(true);

uint32_t compute(const std::string &path) noexcept(false);

} // namespace crc

} // namespace zippuccino