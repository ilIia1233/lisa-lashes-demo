#include "password.h"
#include <argon2.h>
#include <logger/log.h>
#include <stdexcept>
#include <vector>

namespace crypto {

static constexpr uint32_t t_cost = 3;       // iterations
static constexpr uint32_t m_cost = 1 << 16; // 64MB
static constexpr uint32_t parallelism = 1;
static constexpr uint32_t hash_len = 32;
static constexpr uint32_t salt_len = 16;

std::string hash_password(const std::string &password) {

  std::vector<uint8_t> salt(salt_len);
  arc4random_buf(salt.data(), salt_len); // or secure RNG of your choice

  std::vector<uint8_t> hash(hash_len);

  int result = argon2id_hash_raw(t_cost, m_cost, parallelism, password.data(),
                                 password.size(), salt.data(), salt_len,
                                 hash.data(), hash_len);

  if (result != ARGON2_OK) {
    throw std::runtime_error("Argon2 hashing failed");
  }

  char encoded[128];
  argon2id_hash_encoded(t_cost, m_cost, parallelism, password.data(),
                        password.size(), salt.data(), salt_len, hash_len,
                        encoded, sizeof(encoded));

  return std::string(encoded);
}

bool verify_password(const std::string &password,
                     const std::string &stored_hash) {

  int result =
      argon2id_verify(stored_hash.c_str(), password.data(), password.size());

  return result == ARGON2_OK;
}

} // namespace crypto
