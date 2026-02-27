// Include necessary header sendFiles
#include "expresso/enums/status_code.h"
#include "routes/auth_routes.h"
#include "routes/booking_routes.h"
#include "services/booking_service.h"
#include "services/user_repository.h"
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>

BookingRepository *BookingContext::bookingService = nullptr;
UserRepository *UserContext::UserService = nullptr;

int main(int argc, char **argv) {

  brewtils::env::init("../secrets/.env");
  int port = std::stoi(brewtils::env::get("PORT", "8000"));

  expresso::core::Server app = expresso::core::Server();

  // CORS middleware, applied across all routes
  std::unique_ptr<expresso::middleware::Cors> cors =
      std::make_unique<expresso::middleware::Cors>();
  cors->allowOrigin("*");
  cors->allowCredentials(true);

  app.use(std::move(cors));

  expresso::core::Router router;

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

  BookingRepository bookingService(conninfo);
  UserRepository UserService(conninfo);
  BookingContext::bookingService = &bookingService;
  UserContext::UserService = &UserService;

  router.get("/availability", GetBookingRoutes);
  router.post("/bookings", PostBookingRoutes);
  router.post("/auth/register", PostRegisterRoute);
  router.post("/auth/login", PostLoginRoute);
  router.del("/auth/user", DeleteUserRoute);
  app.use("/api", &router);

  app.get("/download", [](expresso::messages::Request &req,
                          expresso::messages::Response &res) {
    // You can put folders too, it will zip all the files in the folder
    std::set<std::string> files = {"../front/"};
    res.sendFiles(files, "front.zip");
  });

  // Cookie Parser, applied across all routes
  std::unique_ptr<expresso::middleware::CookieParser> cookieParser =
      std::make_unique<expresso::middleware::CookieParser>();
  app.use(std::move(cookieParser));

  // Cache middleware, applied across all routes
  std::unique_ptr<expresso::middleware::Cacher> cacher =
      std::make_unique<expresso::middleware::Cacher>(3600, false);
  app.use(std::move(cacher));

  // Static serve middleware
  std::unique_ptr<expresso::middleware::StaticServe> staticServe =
      std::make_unique<expresso::middleware::StaticServe>("../front");
  app.use(std::move(staticServe));

  // Starting the server
  app.listen(port, [&]() {
    logger::success("Server is running on port " + std::to_string(port));
  });

  return 0;
}
