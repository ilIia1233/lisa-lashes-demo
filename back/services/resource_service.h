#pragma once

#include "../db/pg_pool.h"
#include <json/object.h>
#include <string>
#include <vector>

// Embedded service info returned with each resource
struct ResourceService {
  int id = 0;
  std::string name;
};

struct Resource {
  int id = 0;
  std::string name;
  bool active = true;
  std::vector<ResourceService> services;
};

class ResourceRepository {
private:
  PgPool &pool_;

public:
  ResourceRepository(PgPool &pool);

  std::vector<Resource> getAllResources();

  int addResource(const std::string &name);

  void updateResource(int id, json::object obj);

  void deleteResource(int id);
};
