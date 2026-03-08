#pragma once

#include "../services/salon_service.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"

void GetSalonServicesRoute(expresso::messages::Request &req,
                           expresso::messages::Response &res);

void PostSalonServiceRoute(expresso::messages::Request &req,
                           expresso::messages::Response &res);

void PutSalonServiceRoute(expresso::messages::Request &req,
                          expresso::messages::Response &res);

void DeleteSalonServiceRoute(expresso::messages::Request &req,
                             expresso::messages::Response &res);

// PUT /api/resource-services?id=<resource_id>  body: {service_ids:[1,2,...]}
void PutResourceServicesRoute(expresso::messages::Request &req,
                              expresso::messages::Response &res);
