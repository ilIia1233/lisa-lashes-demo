#include "postgres.h"

PgResult::~PgResult() {
  if (res_) PQclear(res_);
}

int PgResult::GetRows() const { return PQntuples(res_); }

int PgResult::GetCols() const { return PQnfields(res_); }

int PgResult::GetAffectedRows() const {
  const char *tuples = PQcmdTuples(res_);
  if (tuples == nullptr || tuples[0] == '\0') return 0;
  return std::stoi(tuples);
}

std::string PgResult::GetEl(int row, int col) const {
  if (PQgetisnull(res_, row, col))
    return "";
  return PQgetvalue(res_, row, col);
}

bool PgResult::CheckStatus() const {
  auto status = PQresultStatus(res_);
  return status == PGRES_TUPLES_OK || status == PGRES_COMMAND_OK;
}

std::string PgResult::PrintError() const { return PQresultErrorMessage(res_); }
