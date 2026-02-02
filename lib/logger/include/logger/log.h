#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <stdexcept>

#include <logger/config.h>

namespace logger {

void success(const std::string message, bool newline = true) noexcept(true);

void info(const std::string message, bool newline = true) noexcept(true);

void debug(const std::string message, bool newline = true) noexcept(true);

void warning(const std::string message, bool newline = true) noexcept(true);

void error(const std::string message,
           const std::string runtimeErrorMessage = "",
           bool newLine = true) noexcept(false);

std::string getTime() noexcept(true);

} // namespace logger