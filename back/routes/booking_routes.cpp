// booking_routes.cpp
#include "booking_routes.h"
#include "../services/booking_service.h"
#include <cstdio>
#include <expresso/core/router.h>
#include <expresso/messages/request.h>
#include <expresso/messages/response.h>
#include <functional>

// Personally, I don't encourange using namespaces, but, I left it here just so
// that the code could be more readable ¯\_(ツ)_/¯
using namespace expresso::core;
using namespace expresso::enums;
using namespace expresso::messages;
using namespace expresso::middleware;

void registerBookingRoutes(expresso::core::Router &router,
                           BookingRepository &bookingService) {
  // ============================
  // GET /api/availability
  // ============================
  router.get("/api/availability", [&](Request &req, Response &res) {
    // Read query param
    std::string date = req.query("date"); // YYYY-MM-DD

    if (date.empty()) {
      res.status(400);
      res.json(R"({"error":"date parameter is required"})");
      return;
    }

    try {
      // Call business logic
      std::string json = bookingService.getAvailability(
          /*resource_id=*/1, date);

      // Send response
      res.status(200);
      res.json(json);

    } catch (const std::exception &e) {
      res.status(500);
      res.json(std::string(R"({"error":")") + e.what() + R"("})");
    }
  });

  // ============================
  // POST /api/bookings
  // ============================
  router.post("/api/bookings", [&](Request &req, Response &res) {
    try {
      // Parse JSON body
      auto body = req.json(); // adapt to YOUR JSON parser

      std::string userId = body["user_id"];
      std::string start = body["start"];
      std::string end = body["end"];

      bookingService.createBooking(
          /*resource_id=*/1, userId, start, end);

      res.status(201);
      res.json(R"({"success":true})");

    } catch (const std::exception &e) {
      res.status(400);
      res.json(std::string(R"({"error":")") + e.what() + R"("})");
    }
  });
}
