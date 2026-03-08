// resource_routes.cpp
#include "resource_routes.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"
#include "logger/log.h"
#include <string>

// GET /api/resources — return all artists/resources
void GetResourcesRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res) {
  try {
    auto resources = ResourceContext::resourceService->getAllResources();

    json::object response;
    response["resources"].resize(0);

    for (const auto &r : resources) {
      json::object item;
      item["id"]     = r.id;
      item["name"]   = r.name;
      item["active"] = r.active ? "true" : "false";

      item["services"].resize(0);
      for (const auto &s : r.services) {
        json::object svc;
        svc["id"]   = s.id;
        svc["name"] = s.name;
        item["services"].push_back(svc);
      }

      response["resources"].push_back(item);
    }

    return res.status(expresso::enums::STATUS_CODE::OK).json(response).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// POST /api/resources — create new artist/resource
void PostResourceRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res) {
  try {
    json::object body = req.body;

    if (body.find("name") == body.end()) {
      json::object err;
      err["message"] = "name is required";
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }

    std::string name = static_cast<std::string>(body["name"]);

    int newId = ResourceContext::resourceService->addResource(name);

    json::object data;
    data["message"] = "Resource created";
    data["id"]      = newId;
    return res.status(expresso::enums::STATUS_CODE::CREATED).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// PUT /api/resources?id= — update artist/resource
void PutResourceRoute(expresso::messages::Request &req,
                      expresso::messages::Response &res) {
  try {
    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int id = std::stoi(idStr);
    json::object body = req.body;

    ResourceContext::resourceService->updateResource(id, body);

    json::object data;
    data["message"] = "Resource updated";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// DELETE /api/resources?id= — delete artist/resource
void DeleteResourceRoute(expresso::messages::Request &req,
                         expresso::messages::Response &res) {
  try {
    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int id = std::stoi(idStr);

    ResourceContext::resourceService->deleteResource(id);

    json::object data;
    data["message"] = "Resource deleted";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}
