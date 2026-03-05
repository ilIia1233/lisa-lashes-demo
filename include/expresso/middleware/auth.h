#pragma once

#include "../../../back/routes/auth_routes.h"
#include "../../../back/services/session_repository.h"
#include "middleware.h"
namespace expresso {

namespace middleware {

class AuthMiddleware : public expresso::middleware::Middleware {
public:
  bool use(expresso::messages::Request &req,
           expresso::messages::Response &res) override;
};

} // namespace middleware
} // namespace expresso
