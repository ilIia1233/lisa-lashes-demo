// schedule_routes.cpp
#include "schedule_routes.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"
#include "json/object.h"
#include "logger/log.h"
#include <string>

// GET /api/schedule?resource_id=
void GetScheduleRoute(expresso::messages::Request  &req,
                      expresso::messages::Response &res) {
  try {
    std::string idStr = req.queries["resource_id"];
    if (idStr.empty())
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();

    int resource_id = std::stoi(idStr);
    auto days = ScheduleContext::scheduleRepo->getWeeklySchedule(resource_id);

    json::object response;
    response["days"].resize(0);
    for (const auto &d : days) {
      json::object obj;
      obj["weekday"] = d.weekday;
      obj["working"] = d.working;
      obj["start"]   = d.start;
      obj["end"]     = d.end;
      response["days"].push_back(obj);
    }
    return res.status(expresso::enums::STATUS_CODE::OK).json(response).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR).end();
  }
}

// PUT /api/schedule?resource_id=
void PutScheduleRoute(expresso::messages::Request  &req,
                      expresso::messages::Response &res) {
  try {
    std::string idStr = req.queries["resource_id"];
    if (idStr.empty())
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();

    int          resource_id = std::stoi(idStr);
    json::object body        = req.body;

    if (body.find("days") == body.end())
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();

    auto &arr = body["days"];
    std::vector<WorkingDay> days;
    for (int i = 0; i < static_cast<int>(arr.size()); i++) {
      WorkingDay d;
      d.weekday = static_cast<int>(arr[i]["weekday"]);
      d.working = static_cast<bool>(arr[i]["working"]);
      d.start   = static_cast<std::string>(arr[i]["start"]);
      d.end     = static_cast<std::string>(arr[i]["end"]);
      days.push_back(d);
    }

    ScheduleContext::scheduleRepo->setWeeklySchedule(resource_id, days);

    json::object data;
    data["message"] = "Schedule saved";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR).end();
  }
}

// GET /api/schedule-overrides?resource_id=&year=&month=
void GetScheduleOverridesRoute(expresso::messages::Request  &req,
                                expresso::messages::Response &res) {
  try {
    std::string idStr    = req.queries["resource_id"];
    std::string yearStr  = req.queries["year"];
    std::string monthStr = req.queries["month"];

    if (idStr.empty())
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();

    int resource_id = std::stoi(idStr);
    int year  = yearStr.empty()  ? 0 : std::stoi(yearStr);
    int month = monthStr.empty() ? 0 : std::stoi(monthStr);

    auto overrides = ScheduleContext::scheduleRepo->getMonthOverrides(
        resource_id, year, month);

    json::object response;
    response["overrides"].resize(0);
    for (const auto &o : overrides) {
      json::object obj;
      obj["date"]    = o.date;
      obj["working"] = o.working;
      obj["start"]   = o.start;
      obj["end"]     = o.end;
      obj["note"]    = o.note;
      response["overrides"].push_back(obj);
    }
    return res.status(expresso::enums::STATUS_CODE::OK).json(response).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR).end();
  }
}

// PUT /api/schedule-overrides?resource_id=
void PutScheduleOverrideRoute(expresso::messages::Request  &req,
                               expresso::messages::Response &res) {
  try {
    std::string idStr = req.queries["resource_id"];
    if (idStr.empty())
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();

    int resource_id = std::stoi(idStr);
    json::object body = req.body;

    std::string date  = (body.find("date")    != body.end())
                             ? static_cast<std::string>(body["date"])  : "";
    bool working      = (body.find("working")  != body.end())
                             ? static_cast<bool>(body["working"])      : false;
    std::string start = (body.find("start")    != body.end())
                             ? static_cast<std::string>(body["start"]) : "";
    std::string end   = (body.find("end")      != body.end())
                             ? static_cast<std::string>(body["end"])   : "";
    std::string note  = (body.find("note")     != body.end())
                             ? static_cast<std::string>(body["note"])  : "";

    if (date.empty())
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();

    ScheduleContext::scheduleRepo->setOverride(resource_id, date, working,
                                               start, end, note);

    json::object data;
    data["message"] = "Override saved";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR).end();
  }
}

// DELETE /api/schedule-overrides?resource_id=&date=
void DeleteScheduleOverrideRoute(expresso::messages::Request  &req,
                                 expresso::messages::Response &res) {
  try {
    std::string idStr   = req.queries["resource_id"];
    std::string dateStr = req.queries["date"];

    if (idStr.empty() || dateStr.empty())
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();

    int resource_id = std::stoi(idStr);
    ScheduleContext::scheduleRepo->deleteOverride(resource_id, dateStr);

    json::object data;
    data["message"] = "Override deleted";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR).end();
  }
}
