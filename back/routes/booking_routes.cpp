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
  std::string resource_id = req.queries["resource_id"];
  std::string customer_name = req.queries["customer_name"];
  std::string status = req.queries["status"];

  json::object response;
  json::object slots;

  response["date"] = date;
  response["resource_id"] = resource_id;

  response["booking"]["start"] = start_time;
  response["booking"]["end"] = end_time;
  response["booking"]["customer_name"] = customer_name;
  response["booking"]["status"] = status;

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
        body.find("customer_name") == body.end() ||
        body.find("resource_id") == body.end() ||
        body.find("status") == body.end()) {

      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(body)
          .end();
    }
    int resource_id = body["resource_id"];
    std::string date = body["date"];
    std::string start = body["start"];
    std::string end = body["end"];
    std::string customer_name = body["customer_name"];
    std::string status = body["status"];

    // Check availability
    if (!Context::bookingService->isFree(resource_id, date, start, end)) {
      return res.status(expresso::enums::STATUS_CODE::CONFLICT)
          .json(body)
          .end();
    }

    // Create booking
    Context::bookingService->addBooking(resource_id, customer_name, date, start,
                                        end, status);

    json::object data;
    data["message"] = "Booking created";

    return res.status(expresso::enums::STATUS_CODE::CREATED).json(data).end();
  } catch (const std::exception &e) {
    json::object data;
    data["message"] = e.what();

    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(data)
        .end();
  }
}
