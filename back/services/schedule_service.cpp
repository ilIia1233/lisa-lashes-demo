#include "schedule_service.h"
#include "logger/log.h"

ScheduleRepository::ScheduleRepository(PgPool &pool) : pool_(pool) {}

/* ─── Weekly schedule ──────────────────────────────────────────── */

std::vector<WorkingDay> ScheduleRepository::getWeeklySchedule(int resource_id) {
  // Initialise all 7 days as non-working with sensible defaults
  std::vector<WorkingDay> days;
  for (int i = 0; i < 7; i++) {
    days.push_back({i, false, "09:00", "18:00"});
  }
  PgConnGuard conn(pool_);
  auto res = conn->exec_params(
      "SELECT weekday, to_char(start_time,'HH24:MI'), "
      "to_char(end_time,'HH24:MI') "
      "FROM working_hours WHERE resource_id = $1 ORDER BY weekday",
      {std::to_string(resource_id)});

  for (int i = 0; i < res.GetRows(); i++) {
    int wd = std::stoi(res.GetEl(i, 0));
    if (wd >= 0 && wd < 7) {
      days[wd].working = true;
      days[wd].start = res.GetEl(i, 1);
      days[wd].end = res.GetEl(i, 2);
    }
  }
  return days;
}

void ScheduleRepository::setWeeklySchedule(
    int resource_id, const std::vector<WorkingDay> &days) {
  PgConnGuard conn(pool_);
  conn->exec_params("DELETE FROM working_hours WHERE resource_id = $1",
                    {std::to_string(resource_id)});

  for (const auto &d : days) {
    if (!d.working)
      continue;
    conn->exec_params("INSERT INTO working_hours (resource_id, weekday, "
                      "start_time, end_time) "
                      "VALUES ($1, $2, $3::time, $4::time)",
                      {std::to_string(resource_id), std::to_string(d.weekday),
                       d.start, d.end});
  }
}

/* ─── Date overrides ──────────────────────────────────────────── */

std::vector<ScheduleOverride>
ScheduleRepository::getMonthOverrides(int resource_id, int year, int month) {
  std::string sql;
  std::vector<std::string> params = {std::to_string(resource_id)};

  if (year > 0 && month > 0) {
    sql = "SELECT to_char(date,'YYYY-MM-DD'), working, "
          "COALESCE(to_char(start_time,'HH24:MI'),''), "
          "COALESCE(to_char(end_time,'HH24:MI'),''), "
          "COALESCE(note,'') "
          "FROM schedule_overrides "
          "WHERE resource_id = $1 "
          "AND EXTRACT(YEAR  FROM date) = $2 "
          "AND EXTRACT(MONTH FROM date) = $3 "
          "ORDER BY date";
    params.push_back(std::to_string(year));
    params.push_back(std::to_string(month));
  } else {
    sql = "SELECT to_char(date,'YYYY-MM-DD'), working, "
          "COALESCE(to_char(start_time,'HH24:MI'),''), "
          "COALESCE(to_char(end_time,'HH24:MI'),''), "
          "COALESCE(note,'') "
          "FROM schedule_overrides "
          "WHERE resource_id = $1 "
          "ORDER BY date";
  }
  PgConnGuard conn(pool_);
  auto res = conn->exec_params(sql, params);

  std::vector<ScheduleOverride> overrides;
  for (int i = 0; i < res.GetRows(); i++) {
    ScheduleOverride o;
    o.date = res.GetEl(i, 0);
    o.working = res.GetEl(i, 1) == "t";
    o.start = res.GetEl(i, 2);
    o.end = res.GetEl(i, 3);
    o.note = res.GetEl(i, 4);
    overrides.push_back(o);
  }
  return overrides;
}

void ScheduleRepository::setOverride(int resource_id, const std::string &date,
                                     bool working, const std::string &start,
                                     const std::string &end,
                                     const std::string &note) {
  PgConnGuard conn(pool_);
  conn->exec_params("INSERT INTO schedule_overrides "
                    "(resource_id, date, working, start_time, end_time, note) "
                    "VALUES ($1, $2::date, $3::boolean, NULLIF($4,'')::time, "
                    "NULLIF($5,'')::time, NULLIF($6,'')) "
                    "ON CONFLICT (resource_id, date) DO UPDATE SET "
                    "working    = EXCLUDED.working, "
                    "start_time = EXCLUDED.start_time, "
                    "end_time   = EXCLUDED.end_time, "
                    "note       = EXCLUDED.note",
                    {std::to_string(resource_id), date,
                     working ? "true" : "false", start, end, note});
}

void ScheduleRepository::deleteOverride(int resource_id,
                                        const std::string &date) {
  PgConnGuard conn(pool_);
  conn->exec_params("DELETE FROM schedule_overrides WHERE resource_id = $1 AND "
                    "date = $2::date",
                    {std::to_string(resource_id), date});
}

/* ─── Effective hours (used by BookingRepository) ─────────────── */

std::pair<bool, std::pair<std::string, std::string>>
ScheduleRepository::getEffectiveHours(int resource_id, const std::string &date,
                                      int weekday) {
  PgConnGuard conn(pool_);
  // 1. Check specific-date override
  auto ores = conn->exec_params(
      "SELECT working, "
      "COALESCE(to_char(start_time,'HH24:MI'),''), "
      "COALESCE(to_char(end_time,'HH24:MI'),'') "
      "FROM schedule_overrides WHERE resource_id=$1 AND date=$2::date",
      {std::to_string(resource_id), date});

  if (ores.GetRows() > 0) {
    bool w = ores.GetEl(0, 0) == "t";
    return {w, {ores.GetEl(0, 1), ores.GetEl(0, 2)}};
  }

  // 2. Fall back to weekly schedule
  auto wres = conn->exec_params(
      "SELECT to_char(start_time,'HH24:MI'), to_char(end_time,'HH24:MI') "
      "FROM working_hours WHERE resource_id=$1 AND weekday=$2",
      {std::to_string(resource_id), std::to_string(weekday)});

  if (wres.GetRows() == 0)
    return {false, {"09:00", "18:00"}};

  return {true, {wres.GetEl(0, 0), wres.GetEl(0, 1)}};
}
