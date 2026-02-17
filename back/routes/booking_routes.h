// booking_routes.h
#pragma once

class Router;
class BookingRepository;

void registerBookingRoutes(Router &router, BookingRepository &bookingService);
