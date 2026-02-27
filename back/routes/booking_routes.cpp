// booking_routes.cpp
#include "booking_routes.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"
#include "logger/log.h"
#include <string>

void GetBookingRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res) {
  try {
    // ============================
    // GET /api/availability
    // ============================

    std::string date = req.queries["date"];
    std::string resourceIdStr = req.queries["resource_id"];

    if (date.empty() || resourceIdStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int resource_id = std::stoi(resourceIdStr);

    // Call service layer
    auto slots =
        Context::bookingService->getAvailableTimeSlots(resource_id, date);

    json::object response;
    response["date"] = date;

    response["slots"].resize(0);

    for (const auto &slot : slots) {
      json::object s;
      s["start"] = slot.start;
      s["end"] = slot.end;
      s["free"] = slot.isFree;

      response["slots"].push_back(s);
    }

    return res.status(expresso::enums::STATUS_CODE::OK).json(response).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
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
    logger::error(e.what());
    json::object data;
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(data)
        .end();
  }
}
