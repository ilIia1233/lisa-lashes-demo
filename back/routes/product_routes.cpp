#include "product_routes.h"
#include "../db/pgconnection.h"
#include "../start_server.h"
#include <brewtils/env.h>
#include <stdexcept>

void GetProductRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res) {
  try {

    auto products = ctx->productRepo.getAllProducts();

    json::object response;
    response["products"].resize(0);

    for (const auto &p : products) {
      json::object item;
      item["id"] = p.id;
      item["name"] = p.name;
      item["description"] = p.description;
      item["price"] = p.price;
      item["stock"] = p.stock;
      item["category"] = p.category;

      response["products"].push_back(item);
    }

    return res.status(expresso::enums::STATUS_CODE::OK).json(response).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

void PostProductRoutes(expresso::messages::Request &req,
                       expresso::messages::Response &res) {
  try {

    json::object body = req.body;

    if (body.find("name") == body.end() || body.find("price") == body.end() ||
        body.find("stock") == body.end()) {
      json::object err;
      err["message"] = "Missing required fields: name, price, stock";
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }

    Product product;
    product.name = static_cast<std::string>(body["name"]);
    product.price = static_cast<std::string>(body["price"]);
    product.stock = static_cast<int>(body["stock"]);

    // Optional fields
    product.description = body.find("description") != body.end()
                              ? static_cast<std::string>(body["description"])
                              : "";

    product.category = body.find("category") != body.end()
                           ? static_cast<std::string>(body["category"])
                           : "Uncategorized";
    ctx->productRepo.addProduct(product);

    json::object data;
    data["message"] = "Product created";

    return res.status(expresso::enums::STATUS_CODE::CREATED).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}

void PutProductRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res) {
  try {

    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int id = std::stoi(idStr);
    json::object body = req.body;

    ctx->productRepo.updateProduct(id, body);

    json::object data;
    data["message"] = "Product updated";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}

void DeleteProductRoutes(expresso::messages::Request &req,
                         expresso::messages::Response &res) {
  try {

    std::string idStr = req.queries["id"];
    if (idStr.empty()) {
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST).end();
    }

    int id = std::stoi(idStr);

    ctx->productRepo.deleteProduct(id);

    json::object data;
    data["message"] = "Product deleted";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .end();
  }
}
