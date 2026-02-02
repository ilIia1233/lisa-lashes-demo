#pragma once

#include <brewtils/os/dir.h>
#include <brewtils/os/file.h>

namespace brewtils {

namespace os {

std::string joinPath(const std::string &path1,
                     const std::string &path2) noexcept(true);

std::string basePath(const std::string &path) noexcept(true);

bool isDocker() noexcept(true);

} // namespace os

} // namespace brewtils
