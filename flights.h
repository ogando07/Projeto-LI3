#ifndef TRABALHO_PRATICO_FLIGHTS_H
#define TRABALHO_PRATICO_FLIGHTS_H

#include <time.h>

typedef struct flight Flight;

// criar e destruir
Flight *flight_create(const char *id, time_t departure, time_t actual_departure, time_t arrival, time_t actual_arrival, const char *gate, const char *status, const char *origin, const char *destination, const char *aircraft, const char *airline, const char *tracking_url);
void flight_destroy(Flight *flight);

// getters
const char *flight_get_id(const Flight *flight);
time_t flight_get_departure(const Flight *flight);
time_t flight_get_actual_departure(const Flight *flight);
time_t flight_get_arrival(const Flight *flight);
time_t flight_get_actual_arrival(const Flight *flight);
const char *flight_get_gate(const Flight *flight);
const char *flight_get_status(const Flight *flight);
const char *flight_get_origin(const Flight *flight);
const char *flight_get_destination(const Flight *flight);
const char *flight_get_aircraft(const Flight *flight);
const char *flight_get_airline(const Flight *flight);
const char *flight_get_tracking_url(const Flight *flight);

#endif

