#include <zippuccino/zipper.h>

zippuccino::Zipper::Zipper() : offset(0), count(0), size(0) { return; }

zippuccino::Zipper::~Zipper() { return; }

void zippuccino::Zipper::add(const std::string &path) noexcept(false) {
  std::string baseName = brewtils::os::basePath(path);
  if (brewtils::os::file::exists(path)) {
    this->paths[baseName] = path;
    return;
  }

  if (brewtils::os::dir::exists(path)) {
    std::set<std::string> paths = brewtils::os::dir::tree(path);
    std::string zipPath;
    std::string systemPath;
    for (const std::string &treePath : paths) {
      systemPath = brewtils::os::joinPath(path, treePath);
      if (brewtils::os::dir::exists(systemPath)) {
        continue;
      }
      zipPath = brewtils::os::joinPath(baseName, treePath);
      this->paths[zipPath] = systemPath;
    }
    return;
  }

  logger::error(
      "Failed to add path: " + path,
      "void zippuccino::Zipper::add(const std::string &path) noexcept(false)");
  return;
}

void zippuccino::Zipper::zip() noexcept(true) {
  this->count = this->paths.size();
  this->iterator = this->paths.begin();
  return;
}

bool zippuccino::Zipper::isFinished() noexcept(true) {
  return this->iterator == this->paths.end();
}

uint64_t zippuccino::Zipper::getSize() noexcept(true) {
  if (this->size > 0) {
    return this->size;
  }

  std::map<std::string, std::string>::iterator originalIterator =
      this->iterator;
  this->iterator = this->paths.begin();
  constexpr uint64_t FILE_INFO_SIZE =
      10 * sizeof(uint32_t) + 18 * sizeof(uint16_t);
  while (!this->isFinished()) {
    this->size += FILE_INFO_SIZE;
    this->size += 2 * this->iterator->first.size();
    this->size += brewtils::os::file::size(this->getCurrentFile());
  }

  this->size += 3 * sizeof(uint32_t) + 5 * sizeof(uint16_t);
  this->iterator = originalIterator;
  return this->size;
}

std::string zippuccino::Zipper::getHeader() noexcept(true) {
  std::string zipPath = this->iterator->first;
  std::string systemPath = this->iterator->second;
  uint32_t fileSize = brewtils::os::file::size(systemPath);
  uint32_t crc32 = zippuccino::crc::compute(systemPath);

  std::ostringstream headers;
  uint16_t pathSize = zipPath.size();
  headers.write(reinterpret_cast<const char *>(
                    &zippuccino::config::LOCAL_FILE_HEADER_SIGNATURE),
                sizeof(uint32_t));
  headers.write(
      reinterpret_cast<const char *>(&zippuccino::config::VERSION_NEEDED),
      sizeof(uint16_t));
  headers.write(reinterpret_cast<const char *>(
                    &zippuccino::config::GENERAL_PURPOSE_BIT_FLAG),
                sizeof(uint16_t));
  headers.write(
      reinterpret_cast<const char *>(&zippuccino::config::COMPRESSION_METHOD),
      sizeof(uint16_t));
  headers.write(
      reinterpret_cast<const char *>(&zippuccino::config::LAST_MOD_TIME),
      sizeof(uint16_t));
  headers.write(
      reinterpret_cast<const char *>(&zippuccino::config::LAST_MOD_DATE),
      sizeof(uint16_t));
  headers.write(reinterpret_cast<const char *>(&crc32), sizeof(uint32_t));
  headers.write(reinterpret_cast<const char *>(&fileSize), sizeof(uint32_t));
  headers.write(reinterpret_cast<const char *>(&fileSize), sizeof(uint32_t));
  headers.write(reinterpret_cast<const char *>(&pathSize), sizeof(uint16_t));
  headers.write(
      reinterpret_cast<const char *>(&zippuccino::config::EXTRA_FIELD_LENGTH),
      sizeof(uint16_t));
  headers.write(zipPath.c_str(), pathSize);

  zippuccino::FileInfo fileInfo;
  fileInfo.name = zipPath;
  fileInfo.size = fileSize;
  fileInfo.offset = this->offset;
  fileInfo.crc32 = crc32;
  this->fileInfos.push_back(fileInfo);

  this->offset += 30 + pathSize + fileSize;
  return headers.str();
}

std::string zippuccino::Zipper::getCurrentFile() noexcept(true) {
  std::string systemPath = this->iterator->second;
  this->iterator++;
  return systemPath;
}

std::string zippuccino::Zipper::getFooter() noexcept(true) {
  uint32_t centralDirectorySize = 0;
  uint16_t fileInfoSize;
  std::ostringstream footer;
  for (const zippuccino::FileInfo &fileInfo : this->fileInfos) {
    centralDirectorySize += 46 + fileInfo.name.size();
    fileInfoSize = fileInfo.name.size();
    footer.write(reinterpret_cast<const char *>(
                     &zippuccino::config::CENTRAL_DIRECTORY_HEADER_SIGNATURE),
                 sizeof(uint32_t));
    footer.write(
        reinterpret_cast<const char *>(&zippuccino::config::VERSION_MADE_BY),
        sizeof(uint16_t));
    footer.write(
        reinterpret_cast<const char *>(&zippuccino::config::VERSION_NEEDED),
        sizeof(uint16_t));
    footer.write(reinterpret_cast<const char *>(
                     &zippuccino::config::GENERAL_PURPOSE_BIT_FLAG),
                 sizeof(uint16_t));
    footer.write(
        reinterpret_cast<const char *>(&zippuccino::config::COMPRESSION_METHOD),
        sizeof(uint16_t));
    footer.write(
        reinterpret_cast<const char *>(&zippuccino::config::LAST_MOD_TIME),
        sizeof(uint16_t));
    footer.write(
        reinterpret_cast<const char *>(&zippuccino::config::LAST_MOD_DATE),
        sizeof(uint16_t));
    footer.write(reinterpret_cast<const char *>(&fileInfo.crc32),
                 sizeof(uint32_t));
    footer.write(reinterpret_cast<const char *>(&fileInfo.size),
                 sizeof(uint32_t));
    footer.write(reinterpret_cast<const char *>(&fileInfo.size),
                 sizeof(uint32_t));
    footer.write(reinterpret_cast<const char *>(&fileInfoSize),
                 sizeof(uint16_t));
    footer.write(
        reinterpret_cast<const char *>(&zippuccino::config::EXTRA_FIELD_LENGTH),
        sizeof(uint16_t));
    footer.write(reinterpret_cast<const char *>(
                     &zippuccino::config::FILE_COMMENT_LENGTH),
                 sizeof(uint16_t));
    footer.write(
        reinterpret_cast<const char *>(&zippuccino::config::DISK_NUMBER_START),
        sizeof(uint16_t));
    footer.write(reinterpret_cast<const char *>(
                     &zippuccino::config::INTERNAL_FILE_ATTRIBUTES),
                 sizeof(uint16_t));
    footer.write(reinterpret_cast<const char *>(
                     &zippuccino::config::EXTERNAL_FILE_ATTRIBUTES),
                 sizeof(uint32_t));
    footer.write(reinterpret_cast<const char *>(&fileInfo.offset),
                 sizeof(uint32_t));
    footer.write(fileInfo.name.c_str(), fileInfoSize);
  }

  fileInfoSize = this->fileInfos.size();
  footer.write(
      reinterpret_cast<const char *>(
          &zippuccino::config::END_OF_CENTRAL_DIRECTORY_RECORD_SIGNATURE),
      sizeof(uint32_t));
  footer.write(
      reinterpret_cast<const char *>(&zippuccino::config::DISK_NUMBER_START),
      sizeof(uint16_t));
  footer.write(
      reinterpret_cast<const char *>(&zippuccino::config::DISK_NUMBER_START),
      sizeof(uint16_t));
  footer.write(reinterpret_cast<const char *>(&fileInfoSize), sizeof(uint16_t));
  footer.write(reinterpret_cast<const char *>(&fileInfoSize), sizeof(uint16_t));
  footer.write(reinterpret_cast<const char *>(&centralDirectorySize),
               sizeof(uint32_t));
  footer.write(reinterpret_cast<const char *>(&this->offset), sizeof(uint32_t));
  footer.write(
      reinterpret_cast<const char *>(&zippuccino::config::FILE_COMMENT_LENGTH),
      sizeof(uint16_t));

  return footer.str();
}
