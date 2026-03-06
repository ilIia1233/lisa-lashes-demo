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
    auto slots = BookingContext::bookingService->getAvailableTimeSlots(
        resource_id, date);

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
    if (!BookingContext::bookingService->isFree(resource_id, date, start,
                                                end)) {
      return res.status(expresso::enums::STATUS_CODE::CONFLICT)
          .json(body)
          .end();
    }

    // Create booking
    BookingContext::bookingService->addBooking(resource_id, customer_name, date,
                                               start, end, status);

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

// GET /api/bookings — return all bookings (admin)
void GetAllBookingsRoute(expresso::messages::Request &req,
                         expresso::messages::Response &res) {
  try {
    auto bookings = BookingContext::bookingService->getAllBookings();

    json::object response;
    response["bookings"].resize(0);

    for (const auto &b : bookings) {
      json::object item;
      item["id"] = b.id;
      item["resource_id"] = b.resource_id;
      item["customer_name"] = b.customer_name;
      item["customer_phone"] = b.customer_phone;
      item["customer_email"] = b.customer_email;
      item["date"] = b.date;
      item["start"] = b.start;
      item["end"] = b.end;
      item["status"] = b.status;

      response["bookings"].push_back(item);
    }

    return res.status(expresso::enums::STATUS_CODE::OK).json(response).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// PUT /api/bookings?id=<id> — update booking (admin)
void PutBookingRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res) {
  try {

    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int id = std::stoi(idStr);
    json::object body = req.body;

    BookingContext::bookingService->updateBooking(id, body);

    json::object data;
    data["message"] = "Booking updated";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// DELETE /api/bookings?id=<id> — delete booking (admin)
void DeleteBookingRoutes(expresso::messages::Request &req,
                         expresso::messages::Response &res) {
  try {

    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int id = std::stoi(idStr);

    BookingContext::bookingService->deleteBooking(id);

    json::object data;
    data["message"] = "Booking deleted";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}
