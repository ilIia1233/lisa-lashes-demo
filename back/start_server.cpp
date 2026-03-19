// Include necessary header sendFiles
#include "start_server.h"
#include "logger/log.h"

std::unique_ptr<AppContext> ctx;
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
  ctx = std::make_unique<AppContext>(dbPool);

  if (!ctx) {
    logger::error("Context not initialized");
  }

  //
  // Public Routes
  //

  router.get("/products", GetProductRoutes);
  AdminRouter.post("/products", PostProductRoutes);
  AdminRouter.put("/products", PutProductRoutes);
  AdminRouter.del("/products", DeleteProductRoutes);

    // Booking routes
    router.get("/availability", GetBookingRoutes);
    router.post("/bookings", PostBookingRoutes); // session checked inline

    // Cart routes
    router.get("/cart", GetCartRoute);
    router.post("/cart/items", PostCartItemRoute);
    router.put("/cart/items", PutCartItemRoute);
    router.del("/cart/items", DeleteCartItemRoute);
    router.post("/cart/checkout", PostCheckoutRoute);

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

  // Public frontend — accessible at both /front/* and /* (root)
  FrontRouter.use(
      std::make_unique<expresso::middleware::StaticServe>("../front"));
  app.use("/front", &FrontRouter);
  app.use(std::make_unique<expresso::middleware::StaticServe>("../front"));

  // Starting the server
  app.listen(port, [&]() {
    logger::success("Server is running on port " + std::to_string(port));
  });

  return 0;
}
