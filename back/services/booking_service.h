#pragma once

#include "../db/pgconnection.h"
#include <chrono>
#include <string>
#include <utility>
#include <vector>

struct TimeSlot {
  std::string start; // ISO 8601
  std::string end;   // ISO 8601
};

class BookingRepository {
public:
  explicit BookingRepository(const std::string &conninfo);

  // checks if slot is free (confirmed bookings only)
  bool isFree(int resource_id, const std::string &start,
              const std::string &end);

  // inserts booking (must be wrapped in service transaction)
  void addBooking(int resource_id, const std::string &user_id,
                  const std::string &start, const std::string &end,
                  const std::string &status = "confirmed");

  // get all bookings for a day
  std::vector<TimeSlot> getBookingsForDay(int resource_id,
                                          const std::string &date // YYYY-MM-DD
  );

  // utility
  static bool isValidDateTime(const std::string &iso);

private:
  PgConnection db;
};
