// Include necessary header sendFiles
#include "db/pg_pool.h"
#include "expresso/core/router.h"
#include "expresso/enums/status_code.h"
#include "expresso/middleware/admin_auth.h"
#include "expresso/middleware/auth_protected.h"
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
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>
#include <expresso/middleware/auth.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>

BookingRepository *BookingContext::bookingService = nullptr;
UserRepository *UserContext::UserService = nullptr;
SessionRepository *UserContext::SessionService = nullptr;
ProductRepository *ProductContext::ProductService = nullptr;

int main(int argc, char **argv) {

  brewtils::env::init("../secrets/.env");
  int port = std::stoi(brewtils::env::get("PORT", "8000"));

  expresso::core::Server app = expresso::core::Server();

  // CORS middleware, applied across all routes
  std::unique_ptr<expresso::middleware::Cors> cors =
      std::make_unique<expresso::middleware::Cors>();
  cors->allowOrigin("http://localhost:8000");
  cors->allowMethod("PUT");
  cors->allowMethod("DELETE");
  app.use(std::move(cors));

  // Cookie Parser must be registered BEFORE any route that reads cookies
  std::unique_ptr<expresso::middleware::CookieParser> cookieParser =
      std::make_unique<expresso::middleware::CookieParser>();
  app.use(std::move(cookieParser));

  expresso::core::Router router;
  expresso::core::Router ProtectedRouter;
  expresso::core::Router
      AdminRouter; // API routes — protected by AdminMiddleware
  expresso::core::Router
      AdminStaticRouter; // Static serve — NO middleware (browser does auth)
  expresso::core::Router FrontRouter;

  AdminRouter.use(std::make_unique<expresso::middleware::AdminMiddleware>());
  ProtectedRouter.use(
      std::make_unique<expresso::middleware::ProtectedMiddleware>());

  std::string host = brewtils::env::get("DB_HOST", "0");
  std::string dbport = brewtils::env::get("DB_PORT", "0");
  std::string dbname = brewtils::env::get("DB_NAME", "0");
  std::string user = brewtils::env::get("DB_USER", "0");
  std::string password = brewtils::env::get("DB_PASSWORD", "0");

  std::string conninfo = "host=" + host +
                         " "
                         "port=" +
                         dbport +
                         " "
                         "dbname=" +
                         dbname +
                         " "
                         "user=" +
                         user +
                         " "
                         "password=" +
                         password;
  PgPool dbPool(conninfo, std::thread::hardware_concurrency());
  BookingRepository bookingService(dbPool);
  BookingContext::bookingService = &bookingService;

  UserRepository UserService(dbPool);
  SessionRepository SessionService(dbPool);
  UserContext::UserService = &UserService;
  UserContext::SessionService = &SessionService;

  CartService cartService(dbPool);
  CartContext::cartService = &cartService;

  ProductRepository productService(dbPool);
  ProductContext::ProductService = &productService;

  ResourceRepository resourceServices(dbPool);
  ResourceContext::resourceService = &resourceServices;

  SalonServiceRepository salonServiceRepo(dbPool);
  SalonServiceContext::salonServiceRepo = &salonServiceRepo;

  ScheduleRepository scheduleRepo(dbPool);
  ScheduleContext::scheduleRepo = &scheduleRepo;

  //
  // Public Routes
  //

  router.get("/products", GetProductRoutes);
  ProtectedRouter.post("/products", PostProductRoutes);
  router.put("/products", PutProductRoutes);
  router.del("/products", DeleteProductRoutes);

  // Booking routes
  router.get("/availability", GetBookingRoutes);
  router.post("/bookings", PostBookingRoutes); // session checked inline

  // Auth routes
  router.post("/auth/register", PostRegisterRoute);
  router.post("/auth/login", PostLoginRoute);
  router.del("/auth/user", DeleteUserRoute);
  router.get("/auth/me", GetMeRoute);
  router.post("/auth/logout", PostLogoutRoute);

  // Public read-only resource and service lists (for the booking form)
  router.get("/resources", GetResourcesRoute);
  router.get("/salon-services", GetSalonServicesRoute);

  //
  // ADMIN ROUTES
  //

  AdminRouter.get("/bookings", GetAllBookingsRoute);
  AdminRouter.put("/bookings", PutBookingRoutes);
  AdminRouter.del("/bookings", DeleteBookingRoutes);

  AdminRouter.get("/users", GetUsersRoute);
  AdminRouter.put("/users", PutUserRoute);
  AdminRouter.del("/users", DeleteUserRoute);

  AdminRouter.get("/resources", GetResourcesRoute);
  AdminRouter.post("/resources", PostResourceRoute);
  AdminRouter.put("/resources", PutResourceRoute);
  AdminRouter.del("/resources", DeleteResourceRoute);

  AdminRouter.get("/salon-services", GetSalonServicesRoute);
  AdminRouter.post("/salon-services", PostSalonServiceRoute);
  AdminRouter.put("/salon-services", PutSalonServiceRoute);
  AdminRouter.del("/salon-services", DeleteSalonServiceRoute);

  AdminRouter.put("/resource-services", PutResourceServicesRoute);

  AdminRouter.get("/schedule", GetScheduleRoute);
  AdminRouter.put("/schedule", PutScheduleRoute);
  AdminRouter.get("/schedule-overrides", GetScheduleOverridesRoute);
  AdminRouter.put("/schedule-overrides", PutScheduleOverrideRoute);
  AdminRouter.del("/schedule-overrides", DeleteScheduleOverrideRoute);

  // CORS preflight (OPTIONS) handlers
  auto optHandler = [](expresso::messages::Request &req,
                       expresso::messages::Response &res) {
    res.set("access-control-allow-methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.status(expresso::enums::STATUS_CODE::NO_CONTEXT).end();
  };
  router.options("/*", optHandler);
  AdminRouter.options("/*", optHandler);

  router.use("/admin", &AdminRouter);
  app.use("/api", &router);

  // Cache middleware
  std::unique_ptr<expresso::middleware::Cacher> cacher =
      std::make_unique<expresso::middleware::Cacher>(3600, false);
  app.use(std::move(cacher));

  AdminStaticRouter.use(
      std::make_unique<expresso::middleware::StaticServe>("../front_admin"));
  app.use("/admin", &AdminStaticRouter);

  // Public frontend
  app.use(std::make_unique<expresso::middleware::StaticServe>("../front"));

  // Starting the server
  app.listen(port, [&]() {
    logger::success("Server is running on port " + std::to_string(port));
  });

  return 0;
}
