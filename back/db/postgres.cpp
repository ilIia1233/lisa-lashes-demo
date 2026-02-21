#include "postgres.h"

PgResult::~PgResult() { PQclear(res_); }

int PgResult::GetRows() const { return PQntuples(res_); }

int PgResult::GetCols() const { return PQnfields(res_); }

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
