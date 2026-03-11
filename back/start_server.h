#pragma once
// Include necessary header sendFiles
#include "db/pg_pool.h"
#include "routes/auth_routes.h"
#include "routes/booking_routes.h"
#include "routes/cart_routes.h"
#include "routes/product_routes.h"
#include "routes/resource_routes.h"
#include "routes/salon_service_routes.h"
#include "routes/schedule_routes.h"

#include "services/booking_service.h"
#include "services/cart_service.h"
#include "services/resource_service.h"
#include "services/salon_service.h"
#include "services/schedule_service.h"
#include "services/session_repository.h"
#include "services/user_repository.h"

#include <brewtils/env.h>
#include <json/object.h>
#include <logger/log.h>

#include <expresso/core/router.h>
#include <expresso/core/server.h>
#include <expresso/enums/status_code.h>
#include <expresso/middleware/admin_auth.h>
#include <expresso/middleware/auth.h>
#include <expresso/middleware/auth_protected.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>
#include <string>

struct AppContext {
  BookingRepository bookingRepo;
  UserRepository userRepo;
  SessionRepository sessionRepo;
  ProductRepository productRepo;
  CartService cartService;
  ResourceRepository resourceRepo;
  SalonServiceRepository salonServiceRepo;
  ScheduleRepository scheduleRepo;

  AppContext(PgPool &pool)
      : bookingRepo(pool), userRepo(pool), sessionRepo(pool), productRepo(pool),
        cartService(pool), resourceRepo(pool), salonServiceRepo(pool),
        scheduleRepo(pool) {
    logger::info("CTX created");
  }
};

extern std::unique_ptr<AppContext> ctx;
