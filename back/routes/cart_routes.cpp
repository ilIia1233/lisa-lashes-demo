// cart_routes.cpp
#include "cart_routes.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"
#include <expresso/messages/cookie.h>
#include <logger/log.h>
#include <optional>
#include <string>

CartService *CartContext::cartService = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// Helper: extract user_id from the HttpOnly session cookie set at login
// ─────────────────────────────────────────────────────────────────────────────

static std::optional<int>
getUserIdFromCookie(expresso::messages::Request &req) {
  for (expresso::messages::Cookie *c : req.cookies) {
    if (c->name == "user_id" && !c->value.empty()) {
      try { return std::stoi(c->value); } catch (...) {}
    }
  }
  return std::nullopt;
}

// ─────────────────────────────────────────────────────────────────────────────
// GET /api/cart?user_id=<id>
// Returns the current cart (creates an empty one if needed).
// ─────────────────────────────────────────────────────────────────────────────

void GetCartRoute(expresso::messages::Request &req,
                  expresso::messages::Response &res) {
  try {
    auto userIdOpt = getUserIdFromCookie(req);
    if (!userIdOpt.has_value()) {
      json::object err;
      err["message"] = "Not authenticated";
      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }
    int userId = userIdOpt.value();
    json::object cart = CartContext::cartService->getCart(userId);

    return res.status(expresso::enums::STATUS_CODE::OK).json(cart).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// POST /api/cart/items
// Body: { "product_id": int, "quantity": int }
// Adds a product to the cart (increments quantity if already present).
// ─────────────────────────────────────────────────────────────────────────────

void PostCartItemRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res) {
  try {
    auto userIdOpt = getUserIdFromCookie(req);
    if (!userIdOpt.has_value()) {
      json::object err;
      err["message"] = "Not authenticated";
      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }

    json::object body = req.body;

    if (body.find("product_id") == body.end() ||
        body.find("quantity")   == body.end()) {
      json::object err;
      err["message"] = "Missing required fields: product_id, quantity";
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }

    int userId    = userIdOpt.value();
    int productId = body["product_id"];
    int quantity  = body["quantity"];

    CartContext::cartService->addToCart(userId, productId, quantity);

    json::object data;
    data["message"] = "Item added to cart";
    return res.status(expresso::enums::STATUS_CODE::CREATED).json(data).end();

  } catch (const std::invalid_argument &e) {
    json::object err;
    err["message"] = std::string(e.what());
    return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
        .json(err)
        .end();
  } catch (const std::runtime_error &e) {
    json::object err;
    err["message"] = std::string(e.what());
    return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
        .json(err)
        .end();
  } catch (const std::exception &e) {
    logger::error(e.what());
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// PUT /api/cart/items
// Body: { "product_id": int, "quantity": int }
// Sets the exact quantity for a cart item.
// ─────────────────────────────────────────────────────────────────────────────

void PutCartItemRoute(expresso::messages::Request &req,
                      expresso::messages::Response &res) {
  try {
    auto userIdOpt = getUserIdFromCookie(req);
    if (!userIdOpt.has_value()) {
      json::object err;
      err["message"] = "Not authenticated";
      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }

    json::object body = req.body;

    if (body.find("product_id") == body.end() ||
        body.find("quantity")   == body.end()) {
      json::object err;
      err["message"] = "Missing required fields: product_id, quantity";
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }

    int userId    = userIdOpt.value();
    int productId = body["product_id"];
    int quantity  = body["quantity"];

    CartContext::cartService->updateQuantity(userId, productId, quantity);

    json::object data;
    data["message"] = "Cart item updated";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::invalid_argument &e) {
    json::object err;
    err["message"] = std::string(e.what());
    return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
        .json(err)
        .end();
  } catch (const std::runtime_error &e) {
    json::object err;
    err["message"] = std::string(e.what());
    return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
        .json(err)
        .end();
  } catch (const std::exception &e) {
    logger::error(e.what());
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// DELETE /api/cart/items?user_id=<id>&product_id=<id>
// Removes a single product from the cart.
// ─────────────────────────────────────────────────────────────────────────────

void DeleteCartItemRoute(expresso::messages::Request &req,
                         expresso::messages::Response &res) {
  try {
    auto userIdOpt = getUserIdFromCookie(req);
    if (!userIdOpt.has_value()) {
      json::object err;
      err["message"] = "Not authenticated";
      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }

    std::string productIdStr = req.queries["product_id"];
    if (productIdStr.empty()) {
      json::object err;
      err["message"] = "Missing required query parameter: product_id";
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }

    int userId    = userIdOpt.value();
    int productId = std::stoi(productIdStr);

    CartContext::cartService->removeItem(userId, productId);

    json::object data;
    data["message"] = "Item removed from cart";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// POST /api/cart/checkout
// Body: { "user_id": int }
// Creates an order, decrements stock, and clears the cart — atomically.
// ─────────────────────────────────────────────────────────────────────────────

void PostCheckoutRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res) {
  try {
    auto userIdOpt = getUserIdFromCookie(req);
    if (!userIdOpt.has_value()) {
      json::object err;
      err["message"] = "Not authenticated";
      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }

    int userId = userIdOpt.value();
    json::object result = CartContext::cartService->checkout(userId);

    return res.status(expresso::enums::STATUS_CODE::CREATED).json(result).end();

  } catch (const std::runtime_error &e) {
    // Business-logic errors (empty cart, out of stock, …)
    json::object err;
    err["message"] = std::string(e.what());
    return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
        .json(err)
        .end();
  } catch (const std::exception &e) {
    logger::error(e.what());
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}
