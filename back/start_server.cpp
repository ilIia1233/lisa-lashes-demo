// Include necessary header sendFiles
#include "expresso/enums/status_code.h"
#include "services/booking_service.h"
#include "json/object.h"
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

  Router router;

  router.get("/", [](Request &req, Response &res) {
    std::string date = req.queries["date"];
    std::string start_time = req.queries["start_time"];
    std::string end_time = req.queries["end_time"];

    json::object response;
    json::object slots;

    response["date"] = date;
    response["slots"].resize(1);

    response["slots"][0]["start"] = start_time;
    response["slots"][0]["end"] = end_time;
    response["slots"][0]["free"] = true;

    response["slots"][1]["start"] = start_time;
    response["slots"][1]["end"] = end_time;
    response["slots"][1]["free"] = true;

    response["slots"][2]["start"] = start_time;
    response["slots"][2]["end"] = end_time;
    response["slots"][2]["free"] = true;

    res.status(STATUS_CODE::OK).json(response).end();
  });

  app.use("/api", &router);

  app.get("/download", [](Request &req, Response &res) {
    // You can put folders too, it will zip all the files in the folder
    std::set<std::string> files = {"../front/"};
    res.sendFiles(files, "front.zip");
  });
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
