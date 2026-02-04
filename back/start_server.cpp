// Include necessary header sendFiles
#include <brewtils/env.h>
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>

// Personally, I don't encourange using namespaces, but, I left it here just so
// that the code could be more readable ¯\_(ツ)_/¯
using namespace expresso::core;
using namespace expresso::enums;
using namespace expresso::messages;
using namespace expresso::middleware;

// Global variable, just for fun :)
int port;

int main(int argc, char **argv) {
  brewtils::env::init("../.env");
  port = std::stoi(brewtils::env::get("PORT", "8000"));

  Server app = Server();

  // CORS middleware, applied across all routes
  std::unique_ptr<expresso::middleware::Cors> cors = std::make_unique<Cors>();
  cors->allowOrigin("*");
  cors->allowCredentials(true);
  app.use(std::move(cors));

  // Cookie Parser, applied across all routes
  std::unique_ptr<expresso::middleware::CookieParser> cookieParser =
      std::make_unique<CookieParser>();
  app.use(std::move(cookieParser));

  // Cache middleware, applied across all routes
  std::unique_ptr<expresso::middleware::Cacher> cacher =
      std::make_unique<Cacher>(3600, false);
  app.use(std::move(cacher));

  // Static serve middleware
  std::unique_ptr<expresso::middleware::StaticServe> staticServe =
      std::make_unique<StaticServe>("../front");
  app.use(std::move(staticServe));

  // Starting the server
  app.listen(port, []() {
    logger::success("Server is running on port " + std::to_string(port));
  });

  return 0;
}
