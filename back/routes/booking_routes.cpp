// booking_routes.cpp
#include "booking_routes.h"
#include "../start_server.h"
#include "auth_routes.h"
#include <string>

void GetBookingRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res) {
  try {
    // ============================
    // GET /api/availability
    // ============================

    std::string date = req.queries["date"];
    std::string resourceIdStr = req.queries["resource_id"];
    std::string durationStr = req.queries["duration"];

    if (date.empty() || resourceIdStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int resource_id = std::stoi(resourceIdStr);
    int slot_minutes = durationStr.empty() ? 60 : std::stoi(durationStr);

    // Call service layer
    auto slots =
        ctx->bookingRepo.getAvailableTimeSlots(resource_id, date, slot_minutes);

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
    // ── Session check ──────────────────────────────────────
    std::string token;
    for (auto *cookie : req.cookies) {
      if (cookie->name == "session") {
        token = cookie->value;
        break;
      }
    }
    if (token.empty()) {
      json::object err;
      err["message"] = "Not authenticated";
      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }
    auto userIdOpt = ctx->sessionRepo.getUserIdFromToken(token);
    if (!userIdOpt) {
      json::object err;
      err["message"] = "Invalid or expired session";
      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }
    // ───────────────────────────────────────────────────────

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
    int service_id = (body.find("service_id") != body.end())
                         ? static_cast<int>(body["service_id"])
                         : 0;

    // Check availability
    if (!ctx->bookingRepo.isFree(resource_id, date, start, end)) {
      return res.status(expresso::enums::STATUS_CODE::CONFLICT)
          .json(body)
          .end();
    }

    // Create booking
    ctx->bookingRepo.addBooking(resource_id, customer_name, date, start, end,
                                status, service_id);

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
    auto bookings = ctx->bookingRepo.getAllBookings();

    json::object response;
    response["bookings"].resize(0);

    for (const auto &b : bookings) {
      json::object item;
      item["id"] = b.id;
      item["resource_id"] = b.resource_id;
      item["service_id"] = b.service_id;
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

    ctx->bookingRepo.updateBooking(id, body);

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

    ctx->bookingRepo.deleteBooking(id);

    json::object data;
    data["message"] = "Booking deleted";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}
