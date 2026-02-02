#include <logger/log.h>

int main(int argc, char **argv) {
  // logger::ENABLED = true;
  // logger::BEAUTIFY = true;
  // logger::SHOW_TIME = true;
  // logger::TIME_FORMAT = "%Y-%m-%d %H:%M:%S";

  logger::success("This is a success message");
  logger::info("This is an info message");
  logger::debug("This is a debug message");
  logger::warning("This is a warning message");
  logger::error("This is an error message");
  // logger::error("This is an error message with a runtime error message",
  //               "This is a runtime error message");

  return EXIT_SUCCESS;
}