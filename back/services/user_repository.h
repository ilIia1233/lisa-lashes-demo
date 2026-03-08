#pragma once

#include "../db/pg_pool.h"
#include "json/object.h"
#include <expresso/core/server.h>
#include <expresso/middleware/cacher.h>
#include <expresso/middleware/cookie_parser.h>
#include <expresso/middleware/cors.h>
#include <expresso/middleware/static_serve.h>
#include <optional>
#include <string>
#include <vector>

struct User {
public:
  std::string first;
  std::string last;
  std::string email;
  std::string phone;
  std::string password;
  int user_id;
};

struct UserInfo {
  int id = 0;
  std::string first_name;
  std::string last_name;
  std::string phone;
  std::string email;
  bool is_admin = false;
};

class UserRepository {
public:
  UserRepository(PgPool &pool);

  std::optional<int> registerUser(const User &user);

  std::optional<int> loginUser(const std::string &identifier,
                               const std::string &password);

  bool removeUser(int id);
  bool isAdmin(int id);
  // Admin: list all users
  std::vector<UserInfo> getAllUsers();

  // Admin: update user fields
  void updateUser(int id, json::object obj);

private:
  PgPool &pool_;
};
