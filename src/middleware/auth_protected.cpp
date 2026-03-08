#include <expresso/middleware/auth_protected.h>
#include <string>

bool expresso::middleware::ProtectedMiddleware::use(
    expresso::messages::Request &req, expresso::messages::Response &res) {
  std::string token;

  for (auto *cookie : req.cookies) {
    if (cookie->name == "session") {
      token = cookie->value;
      break;
    }
  }

  if (token.empty()) {
    res.set("location", "/login");
    res.status(expresso::enums::STATUS_CODE::FOUND).end();
    return false;
  }

  auto userIdOpt = UserContext::SessionService->getUserIdFromToken(token);

  if (!userIdOpt) {
    res.set("location", "/login");
    res.status(expresso::enums::STATUS_CODE::FOUND).end();
    return false;
  }

  req.userId = userIdOpt.value();

  return true;
}
