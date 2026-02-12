#pragma once
#include <libpq-fe.h>
#include <string>

class PgResult {
public:
  explicit PgResult(PGresult *res) : res_(res) {};
  ~PgResult();
  PgResult(const PgResult &) = delete;
  PgResult &operator=(const PgResult &) = delete;

  PgResult(PgResult &&other) noexcept : res_(other.res_) {
    other.res_ = nullptr;
  }

  int GetRows() const;
  int GetCols() const;
  std::string GetEl(int row, int col) const;
  bool CheckStatus() const;
  std::string PrintError() const;

private:
  PGresult *res_;
};
