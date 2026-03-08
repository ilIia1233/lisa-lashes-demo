#pragma once
#include "../db/pg_pool.h"
#include "postgres.h"
#include <optional>
#include <string>

class SessionRepository {
private:
  PgPool &pool_;

public:
  SessionRepository(PgPool &pool);

  std::string createSession(int user_id);

  std::optional<int> getUserIdFromToken(const std::string &token);

  void deleteSession(const std::string &token);
  void deleteAllSessionsForUser(int user_id);
  void deleteExpiredSessions();
};
