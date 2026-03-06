#include "user_repository.h"
#include "../crypt/password.h"
#include <logger/log.h>
#include <string>

UserRepository::UserRepository(const std::string &conninfo) : db(conninfo) {}

bool UserRepository::removeUser(int id) {

  try {
    db.exec_params("DELETE FROM users WHERE id = $1", {std::to_string(id)});
    return true;
  } catch (...) {
    return false;
  }
}

std::optional<int> UserRepository::registerUser(const User &user) {
  try {
    std::string hash = crypto::hash_password(user.password);

    auto res =
        db.exec_params("INSERT INTO users "
                       "(first_name, last_name, phone, address, password_hash) "
                       "VALUES ($1, $2, $3, $4, $5) "
                       "RETURNING id",
                       {user.first, user.last, user.phone,
                        user.email.empty() ? "" : user.email, hash});

    if (res.GetRows() == 0)
      return std::nullopt;

    int userId = std::stoi(res.GetEl(0, 0));

    return userId;
  } catch (const std::exception &e) {
    logger::error(std::string("registerUser failed: ") + e.what());
    return std::nullopt;
  }
}

std::optional<int> UserRepository::loginUser(const std::string &identifier,
                                             const std::string &password) {
  auto res = db.exec_params("SELECT id, password_hash FROM users "
                            "WHERE phone = $1 OR address = $1",
                            {identifier});

  if (res.GetRows() == 0)
    return std::nullopt;

  const std::string stored_hash = res.GetEl(0, 1);
  if (!crypto::verify_password(password, stored_hash))
    return std::nullopt;

  return std::stoi(res.GetEl(0, 0));
}

std::vector<UserInfo> UserRepository::getAllUsers() {
  std::vector<UserInfo> users;

  auto res = db.exec_params("SELECT id, first_name, last_name, phone, address "
                            "FROM users ORDER BY id ASC",
                            {});

  for (int i = 0; i < res.GetRows(); i++) {
    UserInfo u;
    u.id = std::stoi(res.GetEl(i, 0));
    u.first_name = res.GetEl(i, 1);
    u.last_name = res.GetEl(i, 2);
    u.phone = res.GetEl(i, 3);
    u.email = res.GetEl(i, 4);
    users.push_back(u);
  }

  return users;
}

void UserRepository::updateUser(int id, json::object obj) {
  std::string first_name = "";
  std::string last_name = "";
  std::string phone = "";
  std::string email = "";

  if (obj.find("first_name") != obj.end())
    first_name = static_cast<std::string>(obj["first_name"]);

  if (obj.find("last_name") != obj.end())
    last_name = static_cast<std::string>(obj["last_name"]);

  if (obj.find("phone") != obj.end())
    phone = static_cast<std::string>(obj["phone"]);

  if (obj.find("email") != obj.end())
    email = static_cast<std::string>(obj["email"]);

  db.exec_params("UPDATE users SET "
                 "first_name = COALESCE(NULLIF($2, ''), first_name), "
                 "last_name  = COALESCE(NULLIF($3, ''), last_name), "
                 "phone      = COALESCE(NULLIF($4, ''), phone), "
                 "address    = COALESCE(NULLIF($5, ''), address) "
                 "WHERE id = $1",
                 {std::to_string(id), first_name, last_name, phone, email});
}
