// cart_routes.h
#pragma once

#include "../services/cart_service.h"
#include "expresso/enums/status_code.h"
#include "json/object.h"
#include <expresso/core/server.h>

class CartService;

class CartContext {
public:
  static CartService *cartService;
};

// GET  /api/cart                   (user_id from cookie)
void GetCartRoute(expresso::messages::Request &req,
                  expresso::messages::Response &res);

// POST /api/cart/items    body: { product_id, quantity }   (user_id from cookie)
void PostCartItemRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res);

// PUT  /api/cart/items    body: { product_id, quantity }   (user_id from cookie)
void PutCartItemRoute(expresso::messages::Request &req,
                      expresso::messages::Response &res);

// DELETE /api/cart/items  ?product_id=<id>                 (user_id from cookie)
void DeleteCartItemRoute(expresso::messages::Request &req,
                         expresso::messages::Response &res);

// POST /api/cart/checkout                                  (user_id from cookie)
void PostCheckoutRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res);
