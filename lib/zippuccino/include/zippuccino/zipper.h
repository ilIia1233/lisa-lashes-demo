#pragma once

#include <map>
#include <sstream>

#include <brewtils/os.h>
#include <logger/log.h>

#include <zippuccino/crc.h>
#include <zippuccino/fileinfo.h>

namespace zippuccino {

class Zipper;

class Zipper {
private:
  uint32_t offset;
  uint16_t count;
  uint64_t size;

  std::map<std::string, std::string> paths;
  std::map<std::string, std::string>::iterator iterator;
  std::vector<FileInfo> fileInfos;

public:
  Zipper();
  ~Zipper();

  void add(const std::string &path) noexcept(false);
  void zip() noexcept(true);

  bool isFinished() noexcept(true);

  uint64_t getSize() noexcept(true);

  std::string getHeader() noexcept(true);
  std::string getCurrentFile() noexcept(true);
  std::string getFooter() noexcept(true);
};

} // namespace zippuccino