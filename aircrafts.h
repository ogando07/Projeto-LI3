#ifndef TRABALHO_PRATICO_AIRCRAFTS_H
#define TRABALHO_PRATICO_AIRCRAFTS_H

typedef struct aircraft Aircraft;

// criar e destruir
Aircraft *aircraft_create(const char *id, const char *manufacturer, const char *model, int year, int capacity, int range);
void aircraft_destroy(Aircraft *a);

// getters
const char *aircraft_get_id(const Aircraft *a);
const char *aircraft_get_manufacturer(const Aircraft *a);
const char *aircraft_get_model(const Aircraft *a);
int aircraft_get_year(const Aircraft *a);
int aircraft_get_capacity(const Aircraft *a);
int aircraft_get_range(const Aircraft *a);
int aircraft_get_flight_count(const Aircraft *a);

// utility function for incrementing flight count
void aircraft_increment_flight_count(Aircraft *a);

#endif
