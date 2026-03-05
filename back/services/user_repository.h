#pragma once

#include "../db/pgconnection.h"
#include <optional>
#include <string>

struct User {
public:
  std::string first;
  std::string last;
  std::string address;
  std::string phone;
  std::string password;
  std::string user_id;
};

class UserRepository {
public:
  UserRepository(const std::string &conninfo);

  bool registerUser(User user);

  std::optional<std::string> loginUser(const std::string &identifier,
                                       const std::string &password);

  bool removeUser(int id);

private:
  PgConnection db;
  User user;
};
