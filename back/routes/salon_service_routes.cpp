// salon_service_routes.cpp
#include "salon_service_routes.h"
#include "../start_server.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"
#include "logger/log.h"
#include <string>

// GET /api/salon-services
void GetSalonServicesRoute(expresso::messages::Request &req,
                           expresso::messages::Response &res) {
  try {
    auto services = ctx->salonServiceRepo.getAllServices();

    json::object response;
    response["services"].resize(0);

    for (const auto &s : services) {
      json::object item;
      item["id"] = s.id;
      item["name"] = s.name;
      item["duration"] = s.duration;
      item["price"] = s.price;
      item["active"] = s.active ? "true" : "false";
      response["services"].push_back(item);
    }

    return res.status(expresso::enums::STATUS_CODE::OK).json(response).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// POST /api/salon-services
void PostSalonServiceRoute(expresso::messages::Request &req,
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
    std::string duration = (body.find("duration") != body.end())
                               ? static_cast<std::string>(body["duration"])
                               : "";
    std::string price = (body.find("price") != body.end())
                            ? static_cast<std::string>(body["price"])
                            : "";

    ctx->salonServiceRepo.addService(name, duration, price);

    json::object data;
    data["message"] = "Service created";
    return res.status(expresso::enums::STATUS_CODE::CREATED).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// PUT /api/salon-services?id=
void PutSalonServiceRoute(expresso::messages::Request &req,
                          expresso::messages::Response &res) {
  try {
    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int id = std::stoi(idStr);
    ctx->salonServiceRepo.updateService(id, req.body);

    json::object data;
    data["message"] = "Service updated";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// DELETE /api/salon-services?id=
void DeleteSalonServiceRoute(expresso::messages::Request &req,
                             expresso::messages::Response &res) {
  try {
    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int id = std::stoi(idStr);
    ctx->salonServiceRepo.deleteService(id);

    json::object data;
    data["message"] = "Service deleted";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

// PUT /api/resource-services?id=<resource_id>
void PutResourceServicesRoute(expresso::messages::Request &req,
                              expresso::messages::Response &res) {
  try {
    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int resource_id = std::stoi(idStr);
    json::object body = req.body;

    std::vector<int> service_ids;
    if (body.find("service_ids") != body.end()) {
      auto &arr = body["service_ids"];
      for (int i = 0; i < static_cast<int>(arr.size()); i++) {
        service_ids.push_back(static_cast<int>(arr[i]));
      }
    }

    ctx->salonServiceRepo.setServicesForResource(resource_id, service_ids);

    json::object data;
    data["message"] = "Artist services updated";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}
