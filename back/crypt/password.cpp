#include "password.h"
#include <argon2.h>
#include <logger/log.h>
#include <stdexcept>
#include <vector>
#include <sys/random.h>

namespace crypto {

static constexpr uint32_t t_cost = 3;       // iterations
static constexpr uint32_t m_cost = 1 << 16; // 64MB
static constexpr uint32_t parallelism = 1;
static constexpr uint32_t hash_len = 32;
static constexpr uint32_t salt_len = 16;

std::string hash_password(const std::string &password) {

  std::vector<uint8_t> salt(salt_len);
  getrandom(salt.data(), salt_len, 0);

  size_t encoded_len =
      argon2_encodedlen(t_cost, m_cost, parallelism, salt_len, hash_len,
                        Argon2_id);
  std::string encoded(encoded_len, '\0');

  int result =
      argon2id_hash_encoded(t_cost, m_cost, parallelism, password.data(),
                            password.size(), salt.data(), salt_len, hash_len,
                            encoded.data(), encoded_len);

  if (result != ARGON2_OK) {
    throw std::runtime_error(std::string("Argon2 hashing failed: ") +
                             argon2_error_message(result));
  }

  // strip trailing null terminator that argon2 writes
  if (!encoded.empty() && encoded.back() == '\0') {
    encoded.pop_back();
  }

  return encoded;
}

bool verify_password(const std::string &password,
                     const std::string &stored_hash) {

  int result =
      argon2id_verify(stored_hash.c_str(), password.data(), password.size());

  return result == ARGON2_OK;
}

} // namespace crypto
