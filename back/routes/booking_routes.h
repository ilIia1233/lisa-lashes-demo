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

class BookingContext {
public:
  static BookingRepository *bookingService;
};

void GetBookingRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res);
void PostBookingRoutes(expresso::messages::Request &req,
                       expresso::messages::Response &res);
