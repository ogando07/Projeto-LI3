#ifndef TRABALHO_PRATICO_DATABASE_H
#define TRABALHO_PRATICO_DATABASE_H

#include "airports.h"
#include "aircrafts.h"
#include "flights.h"
#include "passengers.h"
#include "reservations.h"
#include <stddef.h>

typedef struct database Database;

// Lifecycle
Database* database_create(void);
void database_destroy(Database* db);

// Add entities (returns 0 on success, -1 on error/duplicate)
int database_add_airport(Database* db, Airport* airport);
int database_add_aircraft(Database* db, Aircraft* aircraft);
int database_add_flight(Database* db, Flight* flight);
int database_add_passenger(Database* db, Passenger* passenger);
int database_add_reservation(Database* db, Reservation* reservation);

// Lookup by ID (returns NULL if not found)
Airport* database_get_airport(Database* db, const char* code);
Aircraft* database_get_aircraft(Database* db, const char* id);
Flight* database_get_flight(Database* db, const char* id);
Passenger* database_get_passenger(Database* db, const char* doc_number);
Reservation* database_get_reservation(Database* db, const char* id);

// Get all entities (for queries that need to iterate)
Airport** database_get_all_airports(Database* db, size_t* count);
Aircraft** database_get_all_aircrafts(Database* db, size_t* count);
Flight** database_get_all_flights(Database* db, size_t* count);
Passenger** database_get_all_passengers(Database* db, size_t* count);
Reservation** database_get_all_reservations(Database* db, size_t* count);

#endif
