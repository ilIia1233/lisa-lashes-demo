#pragma once

#include <cstring>
#include <sstream>
#include <unistd.h>

#include <brewtils/sys.h>
#include <brewtils/url.h>

#include <mochios/enums/method.h>
#include <mochios/messages/request.h>
#include <mochios/messages/response.h>

namespace mochios {

namespace helpers {

namespace client {

std::pair<std::string, std::string>
buildRequest(mochios::messages::Request &request);

void buildResponse(mochios::messages::Response &res,
                   std::stringstream &response);

void buildDefaultResponse(mochios::messages::Response &res, int statusCode);

mochios::messages::Response send(mochios::messages::Request &request,
                                 const int &socket);

} // namespace client

} // namespace helpers

} // namespace mochios
