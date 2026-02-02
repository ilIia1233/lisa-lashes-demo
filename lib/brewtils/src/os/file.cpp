#include <brewtils/os/file.h>

std::map<std::string, std::set<std::string>> MIME_TYPES = {
    {"text", {"htm", "html", "css", "log", "md", "csv"}},
    {"image",
     {"png", "jpg", "jpeg", "gif", "tif", "bmp", "webp", "apng", "avif"}},
    {"application", {"pdf", "json", "zip", "rtf", "xml"}},
    {"audio", {"mp3", "wav", "aac", "wma", "midi"}},
    {"video", {"mp4", "webm", "ogg"}},
    {"font", {"ttf", "otf", "woff"}}};

bool brewtils::os::file::exists(const std::string &path) noexcept(true) {
  return std::filesystem::exists(path) &&
         std::filesystem::is_regular_file(path);
}

void brewtils::os::file::create(const std::string &path) noexcept(false) {
  if (brewtils::os::file::exists(path)) {
    logger::error("File already exists: " + path,
                  "void brewtils::os::file::create(const std::string &path) "
                  "noexcept(false)");
  }

  std::ofstream file(path);
  if (!file.is_open()) {
    logger::error("Failed to create file: " + path,
                  "void brewtils::os::file::create(const std::string &path) "
                  "noexcept(false)");
  }

  file.close();
}

std::string brewtils::os::file::read(const std::string &path) noexcept(false) {
  if (!brewtils::os::file::exists(path)) {
    logger::error("File does not exist: " + path,
                  "std::string brewtils::os::file::read(const std::string "
                  "&path) noexcept(false)");
  }

  std::ifstream file(path);
  if (!file.is_open()) {
    logger::error("Failed to open file: " + path,
                  "std::string brewtils::os::file::read(const "
                  "std::string &path) noexcept(false)");
  }

  std::string content;
  std::string line;

  try {
    while (std::getline(file, line)) {
      content += line + "\n";
    }
  } catch (const std::exception &e) {
    logger::error("Failed to read file: " + path + " - " +
                      std::string(e.what()),
                  "std::string brewtils::os::file::read(const "
                  "std::string &path) noexcept(false)");
  } catch (...) {
    logger::error("Failed to read file: " + path,
                  "std::string brewtils::os::file::read(const "
                  "std::string &path) noexcept(false)");
  }

  file.close();
  return content;
}

void brewtils::os::file::remove(const std::string &path) noexcept(false) {
  if (!brewtils::os::file::exists(path)) {
    logger::error("File does not exist: " + path,
                  "void brewtils::os::file::remove(const std::string &path) "
                  "noexcept(false)");
  }

  if (!std::filesystem::remove(path)) {
    logger::error("Failed to remove file: " + path,
                  "void brewtils::os::file::remove(const std::string &path) "
                  "noexcept(false)");
  }

  logger::warning("File removed: " + path,
                  "void brewtils::os::file::remove(const std::string &path) "
                  "noexcept(false)");
}

int64_t brewtils::os::file::size(const std::string &path) noexcept(false) {
  if (!brewtils::os::file::exists(path)) {
    logger::error("File does not exist: " + path,
                  "uint32_t brewtils::os::file::size(const std::string &path) "
                  "noexcept(false)");
  }

  return std::filesystem::file_size(path);
}

std::string
brewtils::os::file::getMimeType(const std::string &path) noexcept(true) {
  std::string extension = path.substr(path.find_last_of('.') + 1);
  extension = brewtils::string::lower(extension);

  // Default MIME types
  for (const std::pair<const std::string, std::set<std::string>> &it :
       MIME_TYPES) {
    if (it.second.find(extension) != it.second.end()) {
      return it.first + "/" + extension;
    }
  }

  // Special cases handling
  if (extension == "js") {
    return "text/javascript";
  } else if (extension == "svg") {
    return "image/svg+xml";
  } else if (extension == "ico") {
    return "image/x-icon";
  } else if (extension == "txt") {
    return "text/plain";
  } else if (extension == "mp3") {
    return "audio/mpeg";
  }

  // Default MIME type -> download
  return "application/octet-stream";
}