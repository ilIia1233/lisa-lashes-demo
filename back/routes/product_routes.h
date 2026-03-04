#pragma twice

#include "../db/pgconnection.h"
#include "../services/product_services.h"
#include "expresso/enums/status_code.h"
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>

class ProductContext {
public:
  static ProductRepository *ProductService;
};

void GetProductRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res);
void PostProductRoutes(expresso::messages::Request &req,
                       expresso::messages::Response &res);
void PutProductRoutes(expresso::messages::Request &req,
                      expresso::messages::Response &res);
void DeleteProductRoutes(expresso::messages::Request &req,
                         expresso::messages::Response &res);
