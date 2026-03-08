#include "session_repository.h"
#include "../crypt/crypt.h"
#include "brewtils/uuid.h"
#include <brewtils/base64.h>
#include <optional>
#include <string>

std::string generateToken() { return brewtils::uuid::v4(); }

SessionRepository::SessionRepository(PgPool &pool) : pool_(pool) {}

std::string SessionRepository::createSession(int user_id) {
  std::string token = generateToken(); // random base64 token

  std::string token_hash = sha256(token);
  PgConnGuard conn(pool_);
  conn->exec_params("INSERT INTO sessions (user_id, token_hash, expires_at)"
                    "VALUES ($1,$2,NOW()+interval '24 hours')",
                    {std::to_string(user_id), token_hash});

  return token; // send raw token to cookie
}

std::optional<int>
SessionRepository::getUserIdFromToken(const std::string &token) {
  std::string hash = sha256(token);
  PgConnGuard conn(pool_);
  auto result = conn->exec_params("SELECT user_id FROM sessions "
                                  "WHERE token_hash=$1 AND expires_at>NOW()",
                                  {hash});
  if (!result.CheckStatus() || result.GetRows() == 0) {
    return std::nullopt;
  }
  return std::stoi(result.GetEl(0, 0));
}

void SessionRepository::deleteSession(const std::string &token) {
  std::string hash = sha256(token);
  PgConnGuard conn(pool_);
  conn->exec_params("DELETE FROM sessions WHERE token_hash=$1", {hash});
}

void SessionRepository::deleteAllSessionsForUser(int user_id) {
  PgConnGuard conn(pool_);
  conn->exec_params("DELETE FROM sessions WHERE user_id=$1",
                    {std::to_string(user_id)});
}

void SessionRepository::deleteExpiredSessions() {
  PgConnGuard conn(pool_);
  conn->exec("DELETE FROM sessions WHERE expires_at <= NOW()");
}
