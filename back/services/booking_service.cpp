#include "booking_service.h"
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>
#include <regex>
#include <stdexcept>
#include <string>

BookingRepository::BookingRepository(const std::string &conninfo)
    : db(conninfo) {}

bool BookingRepository::isValidDateTime(const std::string &date,
                                        const std::string &start,
                                        const std::string &end) {

  std::regex dateRegex(R"(^\d{4}-\d{2}-\d{2}$)");

  if (!std::regex_match(date, dateRegex)) {
    return 0;
  }
  const char *paramValues[1] = {date.c_str()};

  auto res = db.exec_params("SELECT "
                            "($1 || ' ' || $2)::timestamp < "
                            "($1 || ' ' || $3)::timestamp",
                            {date, start, end});

  if (res.GetEl(0, 0) != "t") {
    return 0;
  }

  return 1;
}

bool BookingRepository::isFree(int resource_id, const std::string date,
                               const std::string &start,
                               const std::string &end) {
  if (!isValidDateTime(date, start, end)) {
    throw std::invalid_argument("Invalid datetime format");
  }

  auto res = db.exec_params("SELECT 1 FROM bookings "
                            "WHERE resource_id = $1 "
                            "AND status = 'confirmed' "
                            "AND tstzrange(start_time, end_time) && "
                            "tstzrange($2::timestamptz, $3::timestamptz)",
                            {std::to_string(resource_id), start, end});

  return res.GetRows() == 0;
}

void BookingRepository::addBooking(int resource_id, const std::string &user_id,
                                   const std::string &date,
                                   const std::string &start,
                                   const std::string &end,
                                   const std::string &status) {
  if (!isValidDateTime(date, start, end)) {
    throw std::invalid_argument("Invalid datetime format");
  }

  db.exec_params("INSERT INTO bookings "
                 "(resource_id, user_id, start_time, end_time, status) "
                 "VALUES ($1, $2, $3::timestamptz, $4::timestamptz, $5)",
                 {std::to_string(resource_id), user_id, start, end, status});
}

std::vector<TimeSlot>
BookingRepository::getBookingsForDay(int resource_id, const std::string &date) {
  // date = YYYY-MM-DD
  std::vector<TimeSlot> slots;

  auto res = db.exec_params("SELECT start_time, end_time "
                            "FROM bookings "
                            "WHERE resource_id = $1 "
                            "AND status = 'confirmed' "
                            "AND start_time::date = $2::date "
                            "ORDER BY start_time",
                            {std::to_string(resource_id), date});

  for (int i = 0; i < res.GetRows(); ++i) {
    slots.push_back({res.GetEl(i, 0), res.GetEl(i, 1)});
  }

  return slots;
}
