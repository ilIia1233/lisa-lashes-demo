#include <brewtils/date.h>

std::string brewtils::date::getCurrentUTC() {
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::ostringstream oss;
  oss << std::put_time(std::gmtime(&now_time), "%a, %d %b %Y %H:%M:%S GMT");
  return oss.str();
}