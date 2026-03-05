// auth_routes.h
#pragma once

#include "../services/session_repository.h"
#include "../services/user_repository.h"
#include "expresso/enums/status_code.h"
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>

class UserRepository;
class UserContext {
public:
  static UserRepository *UserService;
  static SessionRepository *SessionService;
};

void PostRegisterRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res);
void PostLoginRoute(expresso::messages::Request &req,
                    expresso::messages::Response &res);
void DeleteUserRoute(expresso::messages::Request &req,
                     expresso::messages::Response &res);
