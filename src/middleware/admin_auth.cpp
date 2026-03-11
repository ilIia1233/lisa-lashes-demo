#include "../../back/start_server.h"
#include "expresso/enums/status_code.h"
#include <expresso/middleware/admin_auth.h>
namespace expresso {
namespace middleware {

bool AdminMiddleware::use(expresso::messages::Request &req,
                          expresso::messages::Response &res) {

  if (!AuthMiddleware::use(req, res))
    return false;

  bool isAdmin = ctx->userRepo.isAdmin(*req.userId);

  if (!isAdmin) {
    res.status(expresso::enums::STATUS_CODE::FORBIDDEN).end();
    return false;
  }

  return true;
}
} // namespace middleware
} // namespace expresso
