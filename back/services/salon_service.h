#pragma once

#include "../db/pgconnection.h"
#include <json/object.h>
#include <string>
#include <vector>

struct SalonService {
  int         id = 0;
  std::string name;
  std::string duration;
  std::string price;
  bool        active = true;
};

class SalonServiceRepository {
private:
  PgConnection db;

public:
  SalonServiceRepository(const std::string &conninfo);

  std::vector<SalonService> getAllServices();

  void addService(const std::string &name,
                  const std::string &duration,
                  const std::string &price);

  void updateService(int id, json::object obj);

  void deleteService(int id);

  // Set which services an artist (resource) offers — replaces existing list
  void setServicesForResource(int resource_id,
                              const std::vector<int> &service_ids);
};

struct SalonServiceContext {
  static SalonServiceRepository *salonServiceRepo;
};
