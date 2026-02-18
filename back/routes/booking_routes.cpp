// booking_routes.cpp
#include "booking_routes.h"

// Personally, I don't encourange using namespaces, but, I left it here just so
// that the code could be more readable ¯\_(ツ)_/¯
using namespace expresso::core;
using namespace expresso::enums;
using namespace expresso::messages;
using namespace expresso::middleware;

void GetBookingRoutes(Request &req, Response &res) {
  // ============================
  // GET /api/availability
  // ============================

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
}

void PostBookingRoutes(Request &req, Response &res) {
  // ============================
  // POST /api/bookings
  // ============================
  try {
    // Parse JSON body
    json::object body = req.body;

    // Validate required fields
    if (body.find("date") == body.end() || body.find("start") == body.end() ||
        body.find("end") == body.end() ||
        body.find("customer_name") == body.end()) {
      return res.status(STATUS_CODE::BAD_REQUEST).json(body).end();
    }

    std::string date = body["date"];
    std::string start = body["start"];
    std::string end = body["end"];
    std::string name = body["customer_name"];

    // Check availability
    if (!bookingService.isAvailable(date, start, end)) {
      return res.status(STATUS_CODE::CONFLICT).json(body).end();
    }

    // Create booking
    bookingService.createBooking(date, start, end, name);

    json::object data;
    data["message"] = "Booking created";

    return res.status(STATUS_CODE::CREATED).json(data).end();
  } catch (const std::exception &e) {
    json::object data;
    data["message"] = "Booking wasns't created: INTERNAL_SERVER_ERROR";

    return res.status(STATUS_CODE::INTERNAL_SERVER_ERROR).json(data).end();
  }
}
