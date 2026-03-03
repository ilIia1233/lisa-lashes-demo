#pragma once

#include "../db/pgconnection.h"
#include <chrono>
#include <string>
#include <utility>
#include <vector>

struct TimeSlot {
  std::string start;
  std::string end;
  bool isFree;
};

class BookingRepository {
public:
  BookingRepository(const std::string &conninfo);

  // checks if slot is free (confirmed bookings only)
  bool isFree(int resource_id, const std::string date, const std::string &start,
              const std::string &end);

  // inserts booking (must be wrapped in service transaction)
  void addBooking(int resource_id, const std::string &user_id,
                  const std::string &date, const std::string &start,
                  const std::string &end,
                  const std::string &status = "confirmed");
  // Gets working hours from working hours table
  std::pair<std::string, std::string> getWorkingHours(int resource_id,
                                                      int weekday);
  // get all bookings for a day
  std::vector<TimeSlot> getBookingsForDay(int resource_id,
                                          const std::string &date // YYYY-MM-DD
  );

  // utility
  bool isValidDateTime(const std::string &date, const std::string &start,
                       const std::string &end);

  std::vector<TimeSlot> getAvailableTimeSlots(int resource_id,
                                              const std::string &date);

private:
  PgConnection db;
};
