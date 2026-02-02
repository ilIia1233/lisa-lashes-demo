#pragma once

#include <regex>
#include <set>
#include <string>
#include <vector>

namespace brewtils {

namespace string {

std::string join(const std::vector<std::string> &strings,
                 const std::string &delimiter) noexcept(true);

std::string join(const std::set<std::string> &strings,
                 const std::string &delimiter) noexcept(true);

std::vector<std::string> split(const std::string &str,
                               const std::string &delimiter) noexcept(true);

std::string trim(const std::string &str) noexcept(true);

std::string replace(const std::string &str, const std::string &from,
                    const std::string &to) noexcept(true);

std::string lower(const std::string &str) noexcept(true);

std::string upper(const std::string &str) noexcept(true);

} // namespace string

} // namespace brewtils