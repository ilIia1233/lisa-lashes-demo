#pragma once
#include "postgres.h"
#include <libpq-fe.h>
#include <mutex>
#include <string>
#include <vector>

class PgConnection {
public:
  explicit PgConnection(const std::string &conninfo);
  ~PgConnection();

  PgConnection(const PgConnection &) = delete;
  PgConnection &operator=(const PgConnection &) = delete;

  PgResult exec(const std::string &query);

  PgResult exec_params(const std::string &query,
                       const std::vector<std::string> &params);

  void begin();
  void commit();
  void rollback();

private:
  PGconn *conn_;
  std::mutex mtx_;
};
