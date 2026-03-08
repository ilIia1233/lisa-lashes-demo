#include "pg_pool.h"

PgPool::PgPool(const std::string &conninfo, size_t size) {
  for (size_t i = 0; i < size; i++) {
    auto conn = std::make_unique<PgConnection>(conninfo);
    available_.push(conn.get());
    connections_.push_back(std::move(conn));
  }
}

PgConnection *PgPool::acquire() {
  std::unique_lock<std::mutex> lock(mtx_);

  cv_.wait(lock, [&] { return !available_.empty(); });

  PgConnection *conn = available_.front();
  available_.pop();

  return conn;
}

void PgPool::release(PgConnection *conn) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    available_.push(conn);
  }

  cv_.notify_one();
}

PgConnGuard::PgConnGuard(PgPool &pool) : pool_(pool), conn_(pool.acquire()) {}

PgConnGuard::~PgConnGuard() { pool_.release(conn_); }

PgConnection *PgConnGuard::operator->() { return conn_; }

PgConnection &PgConnGuard::get() { return *conn_; }
