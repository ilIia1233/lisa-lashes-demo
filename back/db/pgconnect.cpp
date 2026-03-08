#include "pgconnection.h"
#include <stdexcept>

PgConnection::PgConnection(const std::string &conninfo) {
  conn_ = PQconnectdb(conninfo.c_str());

  if (PQstatus(conn_) != CONNECTION_OK) {
    std::string err = PQerrorMessage(conn_);
    PQfinish(conn_);
    throw std::runtime_error("PostgreSQL connection failed: " + err);
  }
}

PgConnection::~PgConnection() {
  if (conn_) {
    PQfinish(conn_);
    conn_ = nullptr;
  }
}

PgResult PgConnection::exec(const std::string &query) {
  PGresult *res = PQexec(conn_, query.c_str());
  PgResult result(res);

  if (!result.CheckStatus()) {
    throw std::runtime_error(result.PrintError());
  }

  return result;
}

PgResult PgConnection::exec_params(const std::string &query,
                                   const std::vector<std::string> &params) {
  std::vector<const char *> values;
  values.reserve(params.size());

  for (const auto &p : params)
    values.push_back(p.c_str());

  PGresult *res = PQexecParams(conn_, query.c_str(), params.size(), nullptr,
                               values.data(), nullptr, nullptr, 0);

  PgResult result(res);

  if (!result.CheckStatus()) {
    throw std::runtime_error(result.PrintError());
  }

  return result;
}

void PgConnection::begin() { exec("BEGIN"); }

void PgConnection::commit() { exec("COMMIT"); }

void PgConnection::rollback() { exec("ROLLBACK"); }
