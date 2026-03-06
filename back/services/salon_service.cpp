#include "salon_service.h"
#include "logger/log.h"
#include <stdexcept>
#include <string>

SalonServiceRepository *SalonServiceContext::salonServiceRepo = nullptr;

SalonServiceRepository::SalonServiceRepository(const std::string &conninfo)
    : db(conninfo) {}

std::vector<SalonService> SalonServiceRepository::getAllServices() {
  std::vector<SalonService> services;

  auto res = db.exec_params("SELECT id, name, duration, price, active "
                            "FROM salon_services "
                            "ORDER BY id ASC",
                            {});

  for (int i = 0; i < res.GetRows(); i++) {
    SalonService s;
    s.id       = std::stoi(res.GetEl(i, 0));
    s.name     = res.GetEl(i, 1);
    s.duration = res.GetEl(i, 2);
    s.price    = res.GetEl(i, 3);
    s.active   = (res.GetEl(i, 4) == "t");
    services.push_back(s);
  }

  return services;
}

void SalonServiceRepository::addService(const std::string &name,
                                        const std::string &duration,
                                        const std::string &price) {
  db.exec_params("INSERT INTO salon_services (name, duration, price, active) "
                 "VALUES ($1, $2, $3, true)",
                 {name, duration, price});
}

void SalonServiceRepository::updateService(int id, json::object obj) {
  std::string name     = "";
  std::string duration = "";
  std::string price    = "";
  std::string active   = "";

  if (obj.find("name") != obj.end())
    name = static_cast<std::string>(obj["name"]);

  if (obj.find("duration") != obj.end())
    duration = static_cast<std::string>(obj["duration"]);

  if (obj.find("price") != obj.end())
    price = static_cast<std::string>(obj["price"]);

  if (obj.find("active") != obj.end())
    active = static_cast<std::string>(obj["active"]);

  db.exec_params("UPDATE salon_services SET "
                 "name     = COALESCE(NULLIF($2, ''), name), "
                 "duration = COALESCE(NULLIF($3, ''), duration), "
                 "price    = COALESCE(NULLIF($4, ''), price), "
                 "active   = CASE WHEN $5 != '' THEN $5::boolean ELSE active END "
                 "WHERE id = $1",
                 {std::to_string(id), name, duration, price, active});
}

void SalonServiceRepository::deleteService(int id) {
  db.exec_params("DELETE FROM salon_services WHERE id = $1::integer",
                 {std::to_string(id)});
}

void SalonServiceRepository::setServicesForResource(
    int resource_id, const std::vector<int> &service_ids) {
  db.exec_params("DELETE FROM resource_services "
                 "WHERE resource_id = $1::integer",
                 {std::to_string(resource_id)});

  for (int service_id : service_ids) {
    db.exec_params("INSERT INTO resource_services (resource_id, service_id) "
                   "VALUES ($1::integer, $2::integer) "
                   "ON CONFLICT DO NOTHING",
                   {std::to_string(resource_id), std::to_string(service_id)});
  }
}
