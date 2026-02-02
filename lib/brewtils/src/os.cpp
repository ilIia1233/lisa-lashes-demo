#include <brewtils/os.h>

std::string brewtils::os::joinPath(const std::string &path1,
                                   const std::string &path2) noexcept(true) {
  return std::filesystem::path(path1).append(path2).string();
}

std::string brewtils::os::basePath(const std::string &path) noexcept(true) {
  std::string base = std::filesystem::path(path).lexically_normal().string();
  if (base.empty() || base == "") {
    return base;
  }

  if (base.back() == '/') {
    base.pop_back();
  }

  size_t pos = base.find_last_of('/');
  return base.substr(pos + 1, base.size());
}

bool brewtils::os::isDocker() noexcept(true) {
  return brewtils::os::file::exists("/.dockerenv");
}