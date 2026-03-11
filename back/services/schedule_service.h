#pragma once

#include "../db/pg_pool.h"
#include <string>
#include <vector>

struct WorkingDay {
  int weekday; // 0 = Sun, 1 = Mon … 6 = Sat
  bool working;
  std::string start; // "HH:MM"
  std::string end;   // "HH:MM"
};

struct ScheduleOverride {
  std::string date; // "YYYY-MM-DD"
  bool working;
  std::string start;
  std::string end;
  std::string note;
};

class ScheduleRepository {
public:
  ScheduleRepository(PgPool &pool);

  // Returns all 7 days; non-working days have working=false
  std::vector<WorkingDay> getWeeklySchedule(int resource_id);

  // Full replace: delete all rows for this resource then re-insert working days
  void setWeeklySchedule(int resource_id, const std::vector<WorkingDay> &days);

  // All overrides for a calendar month (year/month 0 = all)
  std::vector<ScheduleOverride> getMonthOverrides(int resource_id, int year,
                                                  int month);

  // Upsert one date override
  void setOverride(int resource_id, const std::string &date, bool working,
                   const std::string &start, const std::string &end,
                   const std::string &note);

  // Delete one date override
  void deleteOverride(int resource_id, const std::string &date);

  // Used by BookingRepository — checks overrides first, then working_hours
  // Returns {working, {start, end}}
  std::pair<bool, std::pair<std::string, std::string>>
  getEffectiveHours(int resource_id, const std::string &date, int weekday);

private:
  PgPool &pool_;
};
