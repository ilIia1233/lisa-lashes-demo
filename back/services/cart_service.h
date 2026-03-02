#pragma once

#include "../db/pgconnection.h"
#include <json/object.h>
#include <optional>
#include <string>

class CartService {
public:
  explicit CartService(const std::string &conninfo);

  // Returns cart_id for this user; creates a new cart if one doesn't exist
  int getOrCreateCart(int userId);

  // Returns cart_id only if the cart already exists; never creates one
  std::optional<int> getCartIdIfExists(int userId);

  // Adds quantity to a product in the cart.
  // If the product is already in the cart, increments the quantity.
  // Throws if product doesn't exist or stock is insufficient.
  void addToCart(int userId, int productId, int quantity);

  // Sets the exact quantity for a cart item (quantity must be > 0).
  // Throws if product doesn't exist or stock is insufficient.
  void updateQuantity(int userId, int productId, int quantity);

  // Removes a product line from the cart entirely.
  void removeItem(int userId, int productId);

  // Returns a JSON object with cart_id, user_id, items array, and total.
  json::object getCart(int userId);

  // Transactional checkout:
  //   BEGIN → lock stock (FOR UPDATE) → validate quantities →
  //   INSERT order + order_items → decrement stock → clear cart → COMMIT
  // Throws on empty cart, missing product, or insufficient stock (auto ROLLBACK).
  json::object checkout(int userId);

private:
  PgConnection db;
};
