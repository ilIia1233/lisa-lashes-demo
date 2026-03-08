#pragma once
#include "pgconnection.h"
#include "postgres.h"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

class PgPool;

class PgConnGuard {
public:
  PgConnGuard(PgPool &pool);
  ~PgConnGuard();

  PgConnection *operator->();
  PgConnection &get();

private:
  PgPool &pool_;
  PgConnection *conn_;
};

class PgPool {
public:
  PgPool(const std::string &conninfo, size_t size);

  PgConnection *acquire();
  void release(PgConnection *conn);

private:
  std::vector<std::unique_ptr<PgConnection>> connections_;
  std::queue<PgConnection *> available_;

  std::mutex mtx_;
  std::condition_variable cv_;

  friend class PgConnGuard;
};
