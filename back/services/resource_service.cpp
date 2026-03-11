#include "resource_service.h"
#include "logger/log.h"
#include <stdexcept>
#include <string>

ResourceRepository::ResourceRepository(PgPool &pool) : pool_(pool) {}

std::vector<Resource> ResourceRepository::getAllResources() {
  std::vector<Resource> resources;
  PgConnGuard conn(pool_);
  // Fetch all resources
  auto resData = conn->exec_params("SELECT id, name, active "
                                   "FROM resources "
                                   "ORDER BY id ASC",
                                   {});

  for (int i = 0; i < resData.GetRows(); i++) {
    Resource r;
    r.id = std::stoi(resData.GetEl(i, 0));
    r.name = resData.GetEl(i, 1);
    r.active = (resData.GetEl(i, 2) == "t");
    resources.push_back(r);
  }

  // Fetch service assignments in one query and distribute
  auto svcData =
      conn->exec_params("SELECT rs.resource_id, ss.id, ss.name "
                        "FROM resource_services rs "
                        "JOIN salon_services ss ON rs.service_id = ss.id "
                        "ORDER BY rs.resource_id, ss.id",
                        {});

  for (int i = 0; i < svcData.GetRows(); i++) {
    int rid = std::stoi(svcData.GetEl(i, 0));
    int svc_id = std::stoi(svcData.GetEl(i, 1));
    std::string svc_name = svcData.GetEl(i, 2);

    for (auto &r : resources) {
      if (r.id == rid) {
        r.services.push_back({svc_id, svc_name});
        break;
      }
    }
  }

  return resources;
}

int ResourceRepository::addResource(const std::string &name) {
  PgConnGuard conn(pool_);
  auto res = conn->exec_params(
      "INSERT INTO resources (name, active) VALUES ($1, true) RETURNING id",
      {name});
  return std::stoi(res.GetEl(0, 0));
}

void ResourceRepository::updateResource(int id, json::object obj) {
  std::string name = "";
  std::string active = "";
  PgConnGuard conn(pool_);
  if (obj.find("name") != obj.end())
    name = static_cast<std::string>(obj["name"]);

  if (obj.find("active") != obj.end())
    active = static_cast<std::string>(obj["active"]);

  conn->exec_params(
      "UPDATE resources SET "
      "name   = COALESCE(NULLIF($2, ''), name), "
      "active = CASE WHEN $3 != '' THEN $3::boolean ELSE active END "
      "WHERE id = $1",
      {std::to_string(id), name, active});
}

void ResourceRepository::deleteResource(int id) {
  PgConnGuard conn(pool_);
  conn->exec_params("DELETE FROM resources WHERE id = $1::integer",
                    {std::to_string(id)});
}
