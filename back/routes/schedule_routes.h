#pragma once

#include "../services/schedule_service.h"
#include "expresso/messages/request.h"
#include "expresso/messages/response.h"

// GET  /api/schedule?resource_id=
void GetScheduleRoute(expresso::messages::Request &,
                      expresso::messages::Response &);

// PUT  /api/schedule?resource_id=   body: {days:[{weekday,working,start,end}]}
void PutScheduleRoute(expresso::messages::Request &,
                      expresso::messages::Response &);

// GET  /api/schedule-overrides?resource_id=&year=&month=
void GetScheduleOverridesRoute(expresso::messages::Request &,
                                expresso::messages::Response &);

// PUT  /api/schedule-overrides?resource_id=   body: {date,working,start,end,note}
void PutScheduleOverrideRoute(expresso::messages::Request &,
                               expresso::messages::Response &);

// DELETE /api/schedule-overrides?resource_id=&date=
void DeleteScheduleOverrideRoute(expresso::messages::Request &,
                                  expresso::messages::Response &);
