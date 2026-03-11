// auth_routes.h
#pragma once

#include "../services/session_repository.h"
#include "../services/user_repository.h"
#include "expresso/enums/status_code.h"
#include "json/object.h"
#include <brewtils/env.h>
#include <expresso/core/server.h>

void PostRegisterRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res);
void PostLoginRoute(expresso::messages::Request &req,
                    expresso::messages::Response &res);
void DeleteUserRoute(expresso::messages::Request &req,
                     expresso::messages::Response &res);

// Logout route
void PostLogoutRoute(expresso::messages::Request &req,
                     expresso::messages::Response &res);

// Current-user (session) route
void GetMeRoute(expresso::messages::Request &req,
                expresso::messages::Response &res);

// Admin routes
void GetUsersRoute(expresso::messages::Request &req,
                   expresso::messages::Response &res);
void PutUserRoute(expresso::messages::Request &req,
                  expresso::messages::Response &res);
