// Include necessary header sendFiles
#include "db/pgconnection.h"
#include "expresso/enums/status_code.h"
#include "logger/log.h"
#include "routes/booking_routes.h"
#include "services/booking_service.h"
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>

BookingRepository *Context::bookingService = nullptr;

int main(int argc, char **argv) {

  brewtils::env::init("../.env");
  int port = std::stoi(brewtils::env::get("PORT", "8000"));

  expresso::core::Server app = expresso::core::Server();

  // CORS middleware, applied across all routes
  std::unique_ptr<expresso::middleware::Cors> cors =
      std::make_unique<expresso::middleware::Cors>();
  cors->allowOrigin("*");
  cors->allowCredentials(true);

  app.use(std::move(cors));

  expresso::core::Router router;
  std::string conninfo = brewtils::env::get("CONNINFO", "0");

  if (conninfo == "0") {
    logger::error("Server couldn\'t connect to DB, Check env file");
  }

  PgConnection db(conninfo);
  BookingRepository bookingService(conninfo);
  Context::bookingService = &bookingService;

  router.get("/", GetBookingRoutes);
  app.use("/api", &router);
  router.get("/", PostBookingRoutes);
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
