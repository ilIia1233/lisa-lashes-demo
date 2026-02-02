#pragma once

#include <filesystem>
#include <fstream>
#include <map>

#include <logger/log.h>

#include <brewtils/string.h>

namespace brewtils {

namespace os {

namespace file {

bool exists(const std::string &path) noexcept(true);

void create(const std::string &path) noexcept(false);

std::string read(const std::string &path) noexcept(false);

void remove(const std::string &path) noexcept(false);

int64_t size(const std::string &path) noexcept(false);

std::string getMimeType(const std::string &path) noexcept(true);

} // namespace file

} // namespace os

} // namespace brewtils
