// booking_routes.cpp
#include "booking_routes.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"
#include <string>

void GetBookingRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res) {
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

  res.status(expresso::enums::STATUS_CODE::OK).json(response).end();
}

void PostBookingRoutes(expresso::messages::Request &req,
                       expresso::messages::Response &res) {
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
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(body)
          .end();
    }
    int resource_id = body["resource_id"];
    std::string date = body["date"];
    std::string start = body["start"];
    std::string end = body["end"];
    std::string customer_id = body["customer_id"];
    std::string status = body["status"];

    // Check availability
    if (!Context::bookingService->isFree(resource_id, date, start, end)) {
      return res.status(expresso::enums::STATUS_CODE::CONFLICT)
          .json(body)
          .end();
    }

    // Create booking
    Context::bookingService->addBooking(resource_id, customer_id, start, end,
                                        status);

    json::object data;
    data["message"] = "Booking created";

    return res.status(expresso::enums::STATUS_CODE::CREATED).json(data).end();
  } catch (const std::exception &e) {
    json::object data;
    data["message"] = "Booking wasns't created: INTERNAL_SERVER_ERROR";

    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(data)
        .end();
  }
}
