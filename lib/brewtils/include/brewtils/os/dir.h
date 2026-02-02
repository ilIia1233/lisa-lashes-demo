#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <vector>

#include <logger/log.h>

namespace brewtils {

namespace os {

namespace dir {

bool exists(const std::string &path) noexcept(true);

void create(const std::string &path) noexcept(false);

std::string current() noexcept(true);

std::vector<std::string> list(const std::string &path) noexcept(false);

void remove(const std::string &path) noexcept(false);

std::set<std::string> tree(const std::string &path) noexcept(false);

} // namespace dir

} // namespace os

} // namespace brewtils