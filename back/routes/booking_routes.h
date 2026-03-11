// booking_routes.h

#include "expresso/enums/status_code.h"
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>

void GetBookingRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res);
void PostBookingRoutes(expresso::messages::Request &req,
                       expresso::messages::Response &res);

// Admin routes
void GetAllBookingsRoute(expresso::messages::Request &req,
                         expresso::messages::Response &res);
void PutBookingRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res);
void DeleteBookingRoutes(expresso::messages::Request &req,
                         expresso::messages::Response &res);
