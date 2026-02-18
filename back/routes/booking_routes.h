// booking_routes.h
#include "../services/booking_service.h"
#include "expresso/enums/status_code.h"
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>

class Router;
class BookingRepository;

using namespace expresso::core;
using namespace expresso::enums;
using namespace expresso::messages;
using namespace expresso::middleware;

void GetBookingRoutes(Request &req, Response &res);
void PostBookingRoutes(Request &req, Response &res);
