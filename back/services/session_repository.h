#pragma once
#include "../db/pgconnection.h"
#include "postgres.h"
#include <optional>
#include <string>

class SessionRepository {
private:
  PgConnection db;

public:
  SessionRepository(const std::string &conninfo);

  std::string createSession(int user_id);

  std::optional<int> getUserIdFromToken(const std::string &token);

  void deleteSession(const std::string &token);
};
