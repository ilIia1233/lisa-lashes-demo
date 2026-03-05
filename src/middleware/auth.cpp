#include <expresso/middleware/auth.h>
#include <string>

bool AuthMiddleware::use(expresso::messages::Request &req,
                         expresso::messages::Response &res) {
  std::string token;

  // find session cookie
  for (auto *cookie : req.cookies) {
    if (cookie->name == "session") {
      token = cookie->value;
      break;
    }
  }

  if (token.empty()) {
    json::object err;
    err["message"] = "Not authenticated";
    res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED).json(err).end();
    return false;
  }

  auto userIdOpt = UserContext::SessionService->getUserIdFromToken(token);

  if (!userIdOpt) {
    json::object err;
    err["message"] = "Invalid session";
    res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED).json(err).end();
    return false;
  }

  // attach authenticated user to request
  req.userId = *userIdOpt;

  return true;
}
