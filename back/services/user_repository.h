#pragma once

#include "../db/pgconnection.h"
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>
#include <optional>
#include <string>

struct User {
public:
  std::string first;
  std::string last;
  std::string address;
  std::string phone;
  std::string password;
  int user_id;
};

class UserRepository {
public:
  UserRepository(const std::string &conninfo);

  std::optional<int> registerUser(const User &user);

  std::optional<int> loginUser(const std::string &identifier,
                               const std::string &password);

  bool removeUser(int id);

private:
  PgConnection db;
};
