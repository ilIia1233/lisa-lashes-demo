#pragma once

#include <string>

#define MOCHIOS_VERSION_MAJOR 0
#define MOCHIOS_VERSION_MINOR 1
#define MOCHIOS_VERSION_PATCH 0

namespace mochios {

const std::string version = std::to_string(MOCHIOS_VERSION_MAJOR) + "." +
                            std::to_string(MOCHIOS_VERSION_MINOR) + "." +
                            std::to_string(MOCHIOS_VERSION_PATCH);

}