#include "booking_service.h"
#include "logger/log.h"
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>
#include <regex>
#include <stdexcept>
#include <string>

int getWeekdayFromDate(const std::string &date) {
  std::tm tm = {};
  std::istringstream ss(date);
  ss >> std::get_time(&tm, "%Y-%m-%d");

  std::mktime(&tm);  // normalize
  return tm.tm_wday; // 0=Sunday
}

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

  auto res =
      db.exec_params("SELECT 1 FROM bookings "
                     "WHERE resource_id = $1 "
                     "AND status = 'confirmed' "
                     "AND tstzrange(start_time, end_time) && "
                     "tstzrange("
                     "  (($2::date + $3::time) AT TIME ZONE 'Europe/Dublin'), "
                     "  (($2::date + $4::time) AT TIME ZONE 'Europe/Dublin') "
                     ")",
                     {std::to_string(resource_id), date, start, end});
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

  std::string temp = " ";

  db.exec_params("INSERT INTO bookings "
                 "(resource_id, customer_name, customer_phone, customer_email, "
                 "start_time, end_time, status) "
                 "VALUES ("
                 "$1, $2, $3, $4, "
                 "(($5::date + $6::time) AT TIME ZONE 'Europe/Dublin'), "
                 "(($5::date + $7::time) AT TIME ZONE 'Europe/Dublin'), "
                 "$8"
                 ")",
                 {std::to_string(resource_id), user_id, temp, temp, date, start,
                  end, status});
}
std::pair<std::string, std::string>
BookingRepository::getWorkingHours(int resource_id, int weekday) {

  auto res =
      db.exec_params("SELECT start_time, end_time "
                     "FROM working_hours "
                     "WHERE resource_id = $1 AND weekday = $2",
                     {std::to_string(resource_id), std::to_string(weekday)});

  if (res.GetRows() == 0)
    throw std::runtime_error("No working hours for that day");

  return {res.GetEl(0, 0), res.GetEl(0, 1)};
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

std::vector<TimeSlot>
BookingRepository::getAvailableTimeSlots(int resource_id,
                                         const std::string &date) {

  int weekday = getWeekdayFromDate(date);

  std::pair<std::string, std::string> working;
  try {
    working = getWorkingHours(resource_id, weekday);
  } catch (const std::runtime_error &) {
    // No working hours configured for this weekday — return empty
    return {};
  }

  std::vector<TimeSlot> slots;

  int startHour = std::stoi(working.first.substr(0, 2));
  int endHour = std::stoi(working.second.substr(0, 2));

  for (int hour = startHour; hour < endHour; ++hour) {

    std::string lstart = (hour < 10 ? "0" : "") + std::to_string(hour) + ":00";

    std::string lend =
        (hour + 1 < 10 ? "0" : "") + std::to_string(hour + 1) + ":00";

    auto overlap =
        db.exec_params("SELECT 1 FROM bookings "
                       "WHERE resource_id = $1 "
                       "AND status = 'confirmed' "
                       "AND tstzrange(start_time, end_time) && "
                       "tstzrange("
                       "  (($2::date + $3::time) AT TIME ZONE 'Europe/Dublin'), "
                       "  (($2::date + $4::time) AT TIME ZONE 'Europe/Dublin') "
                       ")",
                       {std::to_string(resource_id), date, lstart, lend});

    bool isFree = overlap.GetRows() == 0;
    slots.push_back({lstart, lend, isFree});
  }
  return slots;
}

std::vector<Booking> BookingRepository::getAllBookings() {
  std::vector<Booking> bookings;

  auto res = db.exec_params(
      "SELECT id, resource_id, customer_name, customer_phone, customer_email, "
      "to_char(start_time AT TIME ZONE 'Europe/Dublin', 'YYYY-MM-DD'), "
      "to_char(start_time AT TIME ZONE 'Europe/Dublin', 'HH24:MI'), "
      "to_char(end_time   AT TIME ZONE 'Europe/Dublin', 'HH24:MI'), "
      "status "
      "FROM bookings "
      "ORDER BY start_time ASC",
      {});

  for (int i = 0; i < res.GetRows(); i++) {
    Booking b;
    b.id = std::stoi(res.GetEl(i, 0));
    b.resource_id = std::stoi(res.GetEl(i, 1));
    b.customer_name = res.GetEl(i, 2);
    b.customer_phone = res.GetEl(i, 3);
    b.customer_email = res.GetEl(i, 4);
    b.date = res.GetEl(i, 5);
    b.start = res.GetEl(i, 6);
    b.end = res.GetEl(i, 7);
    b.status = res.GetEl(i, 8);
    bookings.push_back(b);
  }

  return bookings;
}

void BookingRepository::updateBooking(int id, json::object obj) {
  std::string status = "";
  std::string customer_name = "";
  std::string customer_phone = "";
  std::string customer_email = "";
  std::string date = "";
  std::string start = "";
  std::string end = "";

  if (obj.find("status") != obj.end())
    status = static_cast<std::string>(obj["status"]);

  if (obj.find("customer_name") != obj.end())
    customer_name = static_cast<std::string>(obj["customer_name"]);

  if (obj.find("customer_phone") != obj.end())
    customer_phone = static_cast<std::string>(obj["customer_phone"]);

  if (obj.find("customer_email") != obj.end())
    customer_email = static_cast<std::string>(obj["customer_email"]);

  if (obj.find("date") != obj.end())
    date = static_cast<std::string>(obj["date"]);

  if (obj.find("start") != obj.end())
    start = static_cast<std::string>(obj["start"]);

  if (obj.find("end") != obj.end())
    end = static_cast<std::string>(obj["end"]);

  db.exec_params("UPDATE bookings SET "
                 "status = CASE WHEN $2 != '' THEN $2::booking_status ELSE status END, "
                 "customer_name  = COALESCE(NULLIF($3, ''), customer_name), "
                 "customer_phone = COALESCE(NULLIF($4, ''), customer_phone), "
                 "customer_email = COALESCE(NULLIF($5, ''), customer_email), "
                 "start_time = CASE WHEN $6 != '' AND $7 != '' "
                 "  THEN (($6::date + $7::time) AT TIME ZONE 'Europe/Dublin') "
                 "  ELSE start_time END, "
                 "end_time = CASE WHEN $6 != '' AND $8 != '' "
                 "  THEN (($6::date + $8::time) AT TIME ZONE 'Europe/Dublin') "
                 "  ELSE end_time END "
                 "WHERE id = $1",
                 {std::to_string(id), status, customer_name, customer_phone,
                  customer_email, date, start, end});
}

void BookingRepository::deleteBooking(int id) {
  db.exec_params("DELETE FROM bookings WHERE id = $1::integer",
                 {std::to_string(id)});
}
