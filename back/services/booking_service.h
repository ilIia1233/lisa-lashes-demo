#pragma once

#include "../db/pgconnection.h"
#include "json/object.h"
#include <chrono>
#include <string>
#include <utility>
#include <vector>

struct TimeSlot {
  std::string start;
  std::string end;
  bool isFree;
};

struct Booking {
  int id = 0;
  int resource_id = 0;
  int service_id  = 0;
  std::string customer_name;
  std::string customer_phone;
  std::string customer_email;
  std::string date;   // YYYY-MM-DD (local Dublin time)
  std::string start;  // HH:MM
  std::string end;    // HH:MM
  std::string status;
};

class BookingRepository {
public:
  BookingRepository(const std::string &conninfo);

  // checks if slot is free (confirmed bookings only)
  bool isFree(int resource_id, const std::string date, const std::string &start,
              const std::string &end);

  // inserts booking (must be wrapped in service transaction)
  void addBooking(int resource_id, const std::string &customer_name,
                  const std::string &date, const std::string &start,
                  const std::string &end, const std::string &status,
                  int service_id = 0);
  // Gets working hours from working hours table
  std::pair<std::string, std::string> getWorkingHours(int resource_id,
                                                      int weekday);

  // Checks overrides first, then working_hours; returns {working, {start,end}}
  std::pair<bool, std::pair<std::string, std::string>>
  getEffectiveHours(int resource_id, const std::string &date, int weekday);

  // get all bookings for a day
  std::vector<TimeSlot> getBookingsForDay(int resource_id,
                                          const std::string &date // YYYY-MM-DD
  );

  // utility
  bool isValidDateTime(const std::string &date, const std::string &start,
                       const std::string &end);

  // slot_minutes = duration of service (default 60); step = 30 min
  std::vector<TimeSlot> getAvailableTimeSlots(int resource_id,
                                              const std::string &date,
                                              int slot_minutes = 60);

  // Admin: get all bookings
  std::vector<Booking> getAllBookings();

  // Admin: update a booking by id
  void updateBooking(int id, json::object obj);

  // Admin: delete a booking by id
  void deleteBooking(int id);

private:
  PgConnection db;
};
