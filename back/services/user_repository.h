#pragma once

#include "../db/pgconnection.h"
#include <optional>
#include <string>

class UserRepository {
public:
  UserRepository(const std::string &conninfo);

  bool addUser(const std::string &first, const std::string &last,
               const std::string &address, const std::string &phone,
               const std::string &password_hash);

  bool registerUser(const std::string &first, const std::string &last,
                    const std::string &phone, const std::string &password,
                    const std::string &address = "");

  std::optional<std::string> loginUser(const std::string &identifier,
                                       const std::string &password);

  bool removeUser(int id);

  std::optional<std::string> getPasswordHashByPhone(const std::string &phone);

private:
  PgConnection db;
};
