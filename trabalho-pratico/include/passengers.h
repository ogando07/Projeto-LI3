#ifndef TRABALHO_PRATICO_PASSENGERS_H
#define TRABALHO_PRATICO_PASSENGERS_H

#include <time.h>

typedef struct passenger Passenger;

// criar e destruir
Passenger *passenger_create(const char *document_number, const char *first_name, const char *last_name, time_t dob, const char *nationality, char gender, const char *email, const char *phone, const char *address, const char *photo);
void passenger_destroy(Passenger *passenger);

// getters
const char *passenger_get_document_number(const Passenger *passenger);
const char *passenger_get_first_name(const Passenger *passenger);
const char *passenger_get_last_name(const Passenger *passenger);
time_t passenger_get_dob(const Passenger *passenger);
const char *passenger_get_nationality(const Passenger *passenger);
char passenger_get_gender(const Passenger *passenger);
const char *passenger_get_email(const Passenger *passenger);
const char *passenger_get_phone(const Passenger *passenger);
const char *passenger_get_address(const Passenger *passenger);
const char *passenger_get_photo(const Passenger *passenger);

#endif

