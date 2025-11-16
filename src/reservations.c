#include "../include/reservations.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct reservation {
    char *id;                // número da reserva
    char **flight_ids;       // identificadores dos voos associados à reserva (lista de 1 ou 2)
    char *document_number;   // número do documento de identificação do passageiro associado à reserva
    char *seat;              // número do lugar reservado (e.g., 12A)
    double price;            // preço da reserva
    bool extra_luggage;      // indica se a reserva inclui bagagem extra (true ou false)
    bool priority_boarding;  // indica se a reserva inclui embarque prioritário (true ou false)
    char *qr_code;           // código QR associado à reserva
    size_t flight_count;     // quantos voos estão associados a uma reserva (para controlo)
} Reservation;

// create
Reservation *reservation_create(const char *id, const char **flight_ids, const char *document_number, const char *seat, double price, bool extra_luggage, bool priority_boarding, const char *qr_code, size_t flight_count) {
    if (!id || strlen(id) == 0) return NULL;
    if (flight_count < 1 || flight_count > 2) return NULL; // só 1 ou 2 voos permitidos

    Reservation *reservation = malloc(sizeof(Reservation));
    if (!reservation) return NULL;

    reservation->id = strdup(id);
    reservation->document_number = document_number ? strdup(document_number) : NULL;
    reservation->seat = seat ? strdup(seat) : NULL;
    reservation->price = price;
    reservation->extra_luggage = extra_luggage;
    reservation->priority_boarding = priority_boarding;
    reservation->qr_code = qr_code ? strdup(qr_code) : NULL;
    reservation->flight_count = flight_count;

    // Copiar flight_ids
    reservation->flight_ids = malloc(flight_count * sizeof(char *));
    for (size_t i = 0; i < flight_count; i++) {
        reservation->flight_ids[i] = strdup(flight_ids[i]);
    }

    return reservation;
}

// destroy
void reservation_destroy(Reservation *reservation) {
    if (!reservation) return;

    free(reservation->id);
    free(reservation->document_number);
    free(reservation->seat);
    free(reservation->qr_code);

    // libertar flight_ids
    if (reservation->flight_ids) {
        for (size_t i = 0; i < reservation->flight_count; i++)
            free(reservation->flight_ids[i]);
        free(reservation->flight_ids);
    }

    free(reservation);
}

// getters
const char *reservation_get_id(const Reservation *reservation) { return reservation ? reservation->id : NULL; }

const char *reservation_get_flight_id(const Reservation *reservation, size_t index) {
    if (!reservation || index >= reservation->flight_count) return NULL;
    return reservation->flight_ids[index];
}

const char *reservation_get_document_number(const Reservation *reservation) { return reservation ? reservation->document_number : NULL; }

const char *reservation_get_seat(const Reservation *reservation) { return reservation ? reservation->seat : NULL; }

double reservation_get_price(const Reservation *reservation) { return reservation ? reservation->price : 0.0; }

bool reservation_has_extra_luggage(const Reservation *reservation) { return reservation ? reservation->extra_luggage : false; }

bool reservation_has_priority_boarding(const Reservation *reservation) { return reservation ? reservation->priority_boarding : false; }

const char *reservation_get_qr_code(const Reservation *reservation) { return reservation ? reservation->qr_code : NULL; }

size_t reservation_get_flight_count(const Reservation *reservation) {
    return reservation ? reservation->flight_count : 0;
}
