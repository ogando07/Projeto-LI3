#ifndef TRABALHO_PRATICO_RESERVATIONS_H
#define TRABALHO_PRATICO_RESERVATIONS_H

#include <stddef.h>
#include <stdbool.h>

typedef struct reservation Reservation;

// criar e destruir
Reservation *reservation_create(const char *id, const char **flight_ids, const char *document_number, const char *seat, double price, bool extra_luggage, bool priority_boarding, const char *qr_code, size_t flight_count);
void reservation_destroy(Reservation *reservation);

// getters
const char *reservation_get_id(const Reservation *reservation);
const char *reservation_get_flight_id(const Reservation *reservation, size_t index);
const char *reservation_get_document_number(const Reservation *reservation);
const char *reservation_get_seat(const Reservation *reservation);
double reservation_get_price(const Reservation *reservation);
bool reservation_has_extra_luggage(const Reservation *reservation);
bool reservation_has_priority_boarding(const Reservation *reservation);
const char *reservation_get_qr_code(const Reservation *reservation);
size_t reservation_get_flight_count(const Reservation *reservation);

#endif
