#pragma once

#include "../services/resource_service.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"
#include <json/object.h>
#include <logger/log.h>

void GetResourcesRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res);

void PostResourceRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res);

void PutResourceRoute(expresso::messages::Request &req,
                      expresso::messages::Response &res);

void DeleteResourceRoute(expresso::messages::Request &req,
                         expresso::messages::Response &res);
