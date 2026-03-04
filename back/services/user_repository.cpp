#include "user_repository.h"
#include "../crypt/password.h"
#include <logger/log.h>

UserRepository::UserRepository(const std::string &conninfo) : db(conninfo) {}

bool UserRepository::addUser(const std::string &first, const std::string &last,
                             const std::string &address,
                             const std::string &phone,
                             const std::string &password_hash) {

  try {
    db.exec_params("INSERT INTO users "
                   "(first_name, last_name, address, phone, password_hash) "
                   "VALUES ($1, $2, $3, $4, $5)",
                   {first, last, address, phone, password_hash});

    return true;
  } catch (...) {
    return false;
  }
}

bool UserRepository::removeUser(int id) {

  try {
    db.exec_params("DELETE FROM users WHERE id = $1", {std::to_string(id)});
    return true;
  } catch (...) {
    return false;
  }
}

bool UserRepository::registerUser(const std::string &first,
                                  const std::string &last,
                                  const std::string &phone,
                                  const std::string &password,
                                  const std::string &address) {
  try {
    const std::string hash = crypto::hash_password(password);
    if (address.empty()) {
      db.exec_params(
          "INSERT INTO users (first_name, last_name, phone, password_hash) "
          "VALUES ($1, $2, $3, $4)",
          {first, last, phone, hash});
    } else {
      db.exec_params("INSERT INTO users (first_name, last_name, phone, "
                     "address, password_hash) "
                     "VALUES ($1, $2, $3, $4, $5)",
                     {first, last, phone, address, hash});
    }
    return true;
  } catch (const std::exception &e) {
    logger::error(std::string("registerUser failed: ") + e.what());
    return false;
  }
}

std::optional<std::string>
UserRepository::loginUser(const std::string &identifier,
                          const std::string &password) {
  auto res = db.exec_params("SELECT id, password_hash FROM users "
                            "WHERE phone = $1 OR address = $1",
                            {identifier});

  if (res.GetRows() == 0)
    return std::nullopt;

  const std::string stored_hash = res.GetEl(0, 1);
  if (!crypto::verify_password(password, stored_hash))
    return std::nullopt;

  return res.GetEl(0, 0);
}
