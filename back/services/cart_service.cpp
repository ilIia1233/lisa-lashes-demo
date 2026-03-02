#include "cart_service.h"
#include <logger/log.h>
#include <stdexcept>
#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

CartService::CartService(const std::string &conninfo) : db(conninfo) {}

// ─────────────────────────────────────────────────────────────────────────────
// getOrCreateCart
// ─────────────────────────────────────────────────────────────────────────────

int CartService::getOrCreateCart(int userId) {
  // INSERT if not exists, ignore conflict on UNIQUE(user_id) — race-safe
  db.exec_params(
      "INSERT INTO carts (user_id) VALUES ($1) ON CONFLICT (user_id) DO NOTHING",
      {std::to_string(userId)});

  auto res = db.exec_params("SELECT id FROM carts WHERE user_id = $1",
                            {std::to_string(userId)});

  return std::stoi(res.GetEl(0, 0));
}

// ─────────────────────────────────────────────────────────────────────────────
// getCartIdIfExists
// ─────────────────────────────────────────────────────────────────────────────

std::optional<int> CartService::getCartIdIfExists(int userId) {
  auto res = db.exec_params("SELECT id FROM carts WHERE user_id = $1",
                            {std::to_string(userId)});
  if (res.GetRows() == 0) return std::nullopt;
  return std::stoi(res.GetEl(0, 0));
}

// ─────────────────────────────────────────────────────────────────────────────
// addToCart
// ─────────────────────────────────────────────────────────────────────────────

void CartService::addToCart(int userId, int productId, int quantity) {
  if (quantity <= 0) {
    throw std::invalid_argument("Quantity must be greater than 0");
  }

  // Validate product and check available stock
  auto productRes =
      db.exec_params("SELECT stock FROM products WHERE id = $1",
                     {std::to_string(productId)});

  if (productRes.GetRows() == 0) {
    throw std::runtime_error("Product not found");
  }

  int stock = std::stoi(productRes.GetEl(0, 0));

  int cartId = getOrCreateCart(userId);

  // Check how much is already in the cart so total won't exceed stock
  auto existRes = db.exec_params(
      "SELECT quantity FROM cart_items WHERE cart_id = $1 AND product_id = $2",
      {std::to_string(cartId), std::to_string(productId)});

  int alreadyInCart = (existRes.GetRows() > 0)
                          ? std::stoi(existRes.GetEl(0, 0))
                          : 0;

  if (stock < alreadyInCart + quantity) {
    throw std::runtime_error("Not enough stock available (requested: " +
                             std::to_string(alreadyInCart + quantity) +
                             ", in stock: " + std::to_string(stock) + ")");
  }

  // INSERT or increment quantity if already present (UNIQUE constraint)
  db.exec_params(
      "INSERT INTO cart_items (cart_id, product_id, quantity) "
      "VALUES ($1, $2, $3) "
      "ON CONFLICT (cart_id, product_id) "
      "DO UPDATE SET quantity = cart_items.quantity + EXCLUDED.quantity",
      {std::to_string(cartId), std::to_string(productId),
       std::to_string(quantity)});

  db.exec_params("UPDATE carts SET updated_at = NOW() WHERE id = $1",
                 {std::to_string(cartId)});
}

// ─────────────────────────────────────────────────────────────────────────────
// updateQuantity
// ─────────────────────────────────────────────────────────────────────────────

void CartService::updateQuantity(int userId, int productId, int quantity) {
  if (quantity <= 0) {
    throw std::invalid_argument("Quantity must be greater than 0");
  }

  auto productRes =
      db.exec_params("SELECT stock FROM products WHERE id = $1",
                     {std::to_string(productId)});

  if (productRes.GetRows() == 0) {
    throw std::runtime_error("Product not found");
  }

  int stock = std::stoi(productRes.GetEl(0, 0));
  if (stock < quantity) {
    throw std::runtime_error("Not enough stock available (requested: " +
                             std::to_string(quantity) +
                             ", in stock: " + std::to_string(stock) + ")");
  }

  auto cartIdOpt = getCartIdIfExists(userId);
  if (!cartIdOpt.has_value()) {
    throw std::runtime_error("Item not found in cart");
  }
  int cartId = cartIdOpt.value();

  auto updateRes = db.exec_params(
      "UPDATE cart_items SET quantity = $1 "
      "WHERE cart_id = $2 AND product_id = $3",
      {std::to_string(quantity), std::to_string(cartId),
       std::to_string(productId)});

  if (updateRes.GetAffectedRows() == 0) {
    throw std::runtime_error("Item not found in cart");
  }

  db.exec_params("UPDATE carts SET updated_at = NOW() WHERE id = $1",
                 {std::to_string(cartId)});
}

// ─────────────────────────────────────────────────────────────────────────────
// removeItem
// ─────────────────────────────────────────────────────────────────────────────

void CartService::removeItem(int userId, int productId) {
  auto cartId = getCartIdIfExists(userId);
  if (!cartId.has_value()) return; // no cart exists — nothing to remove

  db.exec_params(
      "DELETE FROM cart_items WHERE cart_id = $1 AND product_id = $2",
      {std::to_string(cartId.value()), std::to_string(productId)});

  db.exec_params("UPDATE carts SET updated_at = NOW() WHERE id = $1",
                 {std::to_string(cartId.value())});
}

// ─────────────────────────────────────────────────────────────────────────────
// getCart
// ─────────────────────────────────────────────────────────────────────────────

json::object CartService::getCart(int userId) {
  json::object cart;
  cart["user_id"]    = userId;
  cart["items"].resize(0);
  cart["item_count"] = 0;
  cart["total"]      = std::string("0.00");

  // Viewing the cart must NOT create one — only addToCart does that
  auto cartIdOpt = getCartIdIfExists(userId);
  if (!cartIdOpt.has_value()) {
    return cart;
  }

  int cartId = cartIdOpt.value();
  cart["cart_id"] = cartId;

  // Subtotals computed in SQL — no floating-point arithmetic in the service
  auto res = db.exec_params(
      "SELECT p.id, p.name, p.price::text, ci.quantity, "
      "       CAST(p.price * ci.quantity AS NUMERIC(10,2))::text AS subtotal "
      "FROM cart_items ci "
      "JOIN products p ON p.id = ci.product_id "
      "WHERE ci.cart_id = $1 "
      "ORDER BY p.name",
      {std::to_string(cartId)});

  for (int i = 0; i < res.GetRows(); i++) {
    json::object item;
    item["product_id"] = res.GetEl(i, 0);
    item["name"]       = res.GetEl(i, 1);
    item["price"]      = res.GetEl(i, 2);
    item["quantity"]   = std::stoi(res.GetEl(i, 3));
    item["subtotal"]   = res.GetEl(i, 4);
    cart["items"].push_back(item);
  }

  auto totalRes = db.exec_params(
      "SELECT CAST(COALESCE(SUM(p.price * ci.quantity), 0) AS NUMERIC(10,2))::text "
      "FROM cart_items ci "
      "JOIN products p ON p.id = ci.product_id "
      "WHERE ci.cart_id = $1",
      {std::to_string(cartId)});

  cart["total"]      = totalRes.GetEl(0, 0);
  cart["item_count"] = res.GetRows();

  return cart;
}

// ─────────────────────────────────────────────────────────────────────────────
// checkout  (full ACID transaction)
// ─────────────────────────────────────────────────────────────────────────────

json::object CartService::checkout(int userId) {
  auto cartIdOpt = getCartIdIfExists(userId);
  if (!cartIdOpt.has_value()) {
    throw std::runtime_error("Cart is empty");
  }
  int cartId = cartIdOpt.value();

  struct ItemInfo {
    int         productId;
    int         quantity;
    std::string priceStr; // raw NUMERIC string from DB — no double conversion
  };

  std::vector<ItemInfo> items;

  try {
    db.begin();

    // Snapshot cart inside the transaction and lock rows against concurrent edits
    auto itemsRes = db.exec_params(
        "SELECT product_id, quantity FROM cart_items WHERE cart_id = $1 FOR UPDATE",
        {std::to_string(cartId)});

    if (itemsRes.GetRows() == 0) {
      db.rollback();
      throw std::runtime_error("Cart is empty");
    }

    // ── Step 1: lock each product row and validate stock ──────────────────
    for (int i = 0; i < itemsRes.GetRows(); i++) {
      int productId = std::stoi(itemsRes.GetEl(i, 0));
      int quantity  = std::stoi(itemsRes.GetEl(i, 1));

      // SELECT ... FOR UPDATE prevents concurrent checkouts from overselling
      auto stockRes = db.exec_params(
          "SELECT stock, price FROM products WHERE id = $1 FOR UPDATE",
          {std::to_string(productId)});

      if (stockRes.GetRows() == 0) {
        db.rollback();
        throw std::runtime_error("Product " + std::to_string(productId) +
                                 " no longer exists");
      }

      int stock = std::stoi(stockRes.GetEl(0, 0));

      if (stock < quantity) {
        db.rollback();
        throw std::runtime_error(
            "Insufficient stock for product " + std::to_string(productId) +
            " (requested: " + std::to_string(quantity) +
            ", available: " + std::to_string(stock) + ")");
      }

      // Store the raw NUMERIC string — Postgres already has the right precision
      items.push_back({productId, quantity, stockRes.GetEl(0, 1)});
    }

    // ── Step 2: create the order — total computed in SQL, no double math ──
    auto orderRes = db.exec_params(
        "INSERT INTO orders (user_id, total, status) "
        "SELECT $1, CAST(SUM(p.price * ci.quantity) AS NUMERIC(10,2)), 'pending' "
        "FROM cart_items ci JOIN products p ON p.id = ci.product_id "
        "WHERE ci.cart_id = $2 "
        "RETURNING id, total::text",
        {std::to_string(userId), std::to_string(cartId)});

    int orderId          = std::stoi(orderRes.GetEl(0, 0));
    std::string totalStr = orderRes.GetEl(0, 1);

    // ── Step 3: insert order_items and decrement stock ────────────────────
    for (const auto &item : items) {
      db.exec_params(
          "INSERT INTO order_items "
          "(order_id, product_id, price_at_purchase, quantity) "
          "VALUES ($1, $2, $3, $4)",
          {std::to_string(orderId), std::to_string(item.productId),
           item.priceStr, std::to_string(item.quantity)});

      db.exec_params("UPDATE products SET stock = stock - $1 WHERE id = $2",
                     {std::to_string(item.quantity),
                      std::to_string(item.productId)});
    }

    // ── Step 4: clear the cart ────────────────────────────────────────────
    db.exec_params("DELETE FROM cart_items WHERE cart_id = $1",
                   {std::to_string(cartId)});

    db.commit();

    json::object result;
    result["order_id"] = orderId;
    result["total"]    = totalStr;
    result["status"]   = std::string("pending");
    result["message"]  = std::string("Order placed successfully");

    return result;

  } catch (const std::exception &e) {
    // Attempt rollback; if we already called rollback manually it is a no-op
    try { db.rollback(); } catch (...) {}
    throw;
  }
}
