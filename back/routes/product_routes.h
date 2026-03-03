#pragma once

#include "../db/pgconnection.h"
#include "../services/product_services.h"
#include "expresso/enums/status_code.h"
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>

using namespace expresso::core;
using namespace expresso::enums;
using namespace expresso::messages;
class ProductContext {
public:
  static ProductRepository *ProductService;
};

void GetProductsRoute(Request &req, Response &res);
void PostProductRoute(Request &req, Response &res);
void PutProductRoute(Request &req, Response &res);
void DeleteProductRoute(Request &req, Response &res);
