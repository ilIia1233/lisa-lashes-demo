#include <logger/log.h>

void logger::success(const std::string message, bool newline) noexcept(true) {
  if (ENABLED) {
    std::cout << getTime();
    if (BEAUTIFY)
      std::cout << GREEN;
    std::cout << "[SUCCESS]";
    if (BEAUTIFY)
      std::cout << RESET;
    std::cout << ": " << message;
    if (newline)
      std::cout << "\n";
  }
  std::cout.flush();
}

void logger::info(const std::string message, bool newline) noexcept(true) {
  if (ENABLED) {
    std::cout << getTime();
    if (BEAUTIFY)
      std::cout << BLUE;
    std::cout << "[INFO]   ";
    if (BEAUTIFY)
      std::cout << RESET;
    std::cout << ": " << message;
    if (newline)
      std::cout << "\n";
  }
  std::cout.flush();
}

void logger::debug(const std::string message, bool newline) noexcept(true) {
  if (ENABLED) {
    std::cout << getTime();
    if (BEAUTIFY)
      std::cout << MAGENTA;
    std::cout << "[DEBUG]  ";
    if (BEAUTIFY)
      std::cout << RESET;
    std::cout << ": " << message;
    if (newline)
      std::cout << "\n";
  }
  std::cout.flush();
}

void logger::warning(const std::string message, bool newline) noexcept(true) {
  if (ENABLED) {
    std::cout << getTime();
    if (BEAUTIFY)
      std::cout << YELLOW;
    std::cout << "[WARNING]";
    if (BEAUTIFY)
      std::cout << RESET;
    std::cout << ": " << message;
    if (newline)
      std::cout << "\n";
  }
  std::cout.flush();
}

void logger::error(const std::string message,
                   const std::string runtimeErrorMessage,
                   bool newline) noexcept(false) {
  if (ENABLED) {
    std::cerr << getTime();
    if (BEAUTIFY)
      std::cerr << RED;
    std::cerr << "[ERROR]  ";
    if (BEAUTIFY)
      std::cerr << RESET;
    std::cerr << ": " << message;
    if (newline)
      std::cerr << "\n";
  }
  if (runtimeErrorMessage != "") {
    throw std::runtime_error(runtimeErrorMessage);
  }
  std::cout.flush();
}

std::string logger::getTime() noexcept(true) {
  if (!SHOW_TIME)
    return "";

  time_t currentTime =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char timeStr[100];
  std::strftime(timeStr, sizeof(timeStr), TIME_FORMAT.c_str(),
                std::localtime(&currentTime));
  std::string output(timeStr);
  return "[" + output + "]";
}