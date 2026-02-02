#pragma once

#include <cstdint>

namespace zippuccino {

namespace config {

static const uint32_t POLYNOMIAL = 0xEDB88320;

static const uint32_t LOCAL_FILE_HEADER_SIGNATURE = 0x04034b50;
static const uint32_t CENTRAL_DIRECTORY_HEADER_SIGNATURE = 0x02014b50;
static const uint32_t END_OF_CENTRAL_DIRECTORY_RECORD_SIGNATURE = 0x06054b50;
static const uint16_t VERSION_NEEDED = 20;
static const uint16_t VERSION_MADE_BY = 20;
static const uint16_t GENERAL_PURPOSE_BIT_FLAG = 0;
static const uint16_t COMPRESSION_METHOD = 0;
static const uint16_t LAST_MOD_TIME = 0;
static const uint16_t LAST_MOD_DATE = 0;
static const uint16_t EXTRA_FIELD_LENGTH = 0;
static const uint16_t FILE_COMMENT_LENGTH = 0;
static const uint16_t DISK_NUMBER_START = 0;
static const uint16_t INTERNAL_FILE_ATTRIBUTES = 0;
static const uint32_t EXTERNAL_FILE_ATTRIBUTES = 0;

} // namespace config

} // namespace zippuccino