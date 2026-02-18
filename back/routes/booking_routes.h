// booking_routes.h
#pragma once
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

void registerBookingRoutes(Router &router, BookingRepository &bookingService);
