#include <brewtils/os/dir.h>

bool brewtils::os::dir::exists(const std::string &path) noexcept(true) {
  return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

void brewtils::os::dir::create(const std::string &path) noexcept(false) {
  if (brewtils::os::dir::exists(path)) {
    logger::error("Path already exists or is not a directory " + path,
                  "void brewtils::os::dir::create(const std::string &path) "
                  "noexcept(false)");
  }

  std::filesystem::create_directory(path);
}

std::string brewtils::os::dir::current() noexcept(true) {
  return std::filesystem::current_path().string();
}

std::vector<std::string>
brewtils::os::dir::list(const std::string &path) noexcept(false) {
  if (!brewtils::os::dir::exists(path)) {
    logger::error("Path does not exist or is not a directory " + path,
                  "std::vector<std::string> brewtils::os::dir::list(const "
                  "std::string &path) noexcept(false)");
  }

  std::vector<std::string> files;
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::directory_iterator(path)) {
    files.push_back(entry.path().string());
  }
  return files;
}

void brewtils::os::dir::remove(const std::string &path) noexcept(false) {
  if (!brewtils::os::dir::exists(path)) {
    logger::error("Path does not exist or is not a directory " + path,
                  "void brewtils::os::dir::remove(const std::string &path) "
                  "noexcept(false)");
  }

  std::filesystem::remove_all(path);
}

std::set<std::string>
brewtils::os::dir::tree(const std::string &path) noexcept(false) {
  if (!brewtils::os::dir::exists(path)) {
    logger::error("Path does not exist or is not a directory " + path,
                  "std::set<std::string> brewtils::os::dir::tree(const "
                  "std::string &path) "
                  "noexcept(false)");
  }

  std::string relativePath;
  std::set<std::string> files;
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::recursive_directory_iterator(path)) {
    try {
      if (!entry.is_regular_file() && !entry.is_directory()) {
        continue;
      }

      relativePath = std::filesystem::relative(entry.path(), path).string();
      files.insert(relativePath);
    } catch (const std::filesystem::filesystem_error &e) {
      continue;
    }
  }

  return files;
}