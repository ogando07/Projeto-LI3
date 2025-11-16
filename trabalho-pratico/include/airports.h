#ifndef TRABALHO_PRATICO_AIRPORTS_H
#define TRABALHO_PRATICO_AIRPORTS_H

#include <stddef.h>

typedef struct airport Airport;

Airport *airport_create(const char *code, const char *name, const char *city, const char *country, double latitude, double longitude, const char *icao, const char *type);
void airport_destroy(Airport *a);

// getters
const char *airport_get_code(Airport *a);
const char *airport_get_name(Airport *a);
const char *airport_get_city(Airport *a);
const char *airport_get_country(Airport *a);
double airport_get_latitude(Airport *a);
double airport_get_longitude(Airport *a);
const char *airport_get_icao(Airport *a);
const char *airport_get_type(Airport *a);

// utility queries
size_t airport_get_departures_count(const Airport *a);
void airport_increment_departures_count(Airport *a);

#endif
