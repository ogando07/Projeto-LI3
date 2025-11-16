#include "../include/flights.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef struct flight {
    char *id;                    // identificador único do voo
    time_t departure;            // data e hora de partida estimada do voo
    time_t actual_departure;     // data e hora de partida real do voo
    time_t arrival;              // data e hora de chegada estimada do voo
    time_t actual_arrival;       // data e hora de chegada real do voo
    char *gate;                  // porta de embarque do voo
    char *status;                // estado do voo, i.e., On Time, Delayed ou Cancelled
    char *origin;                // código IATA do aeroporto de origem
    char *destination;           // código IATA do aeroporto de destino
    char *aircraft;              // dentificador único da aeronave utilizada no voo, i.e., tail number
    char *airline;               // nome da companhia aérea responsável pelo voo
    char *tracking_url;          // URL para o rastreamento do voo
} Flight;

// criar
Flight *flight_create(const char *id, time_t departure, time_t actual_departure, time_t arrival, time_t actual_arrival, const char *gate, const char *status, const char *origin, const char *destination, const char *aircraft, const char *airline, const char *tracking_url) {
    if (!id || strlen(id) == 0) return NULL;

    Flight *flight = malloc(sizeof(Flight));
    if (!flight) return NULL;

    flight->id = strdup(id);
    flight->departure = departure;
    flight->actual_departure = actual_departure;
    flight->arrival = arrival;
    flight->actual_arrival = actual_arrival;
    flight->gate = gate ? strdup(gate) : NULL;
    flight->status = status ? strdup(status) : NULL;
    flight->origin = origin ? strdup(origin) : NULL;
    flight->destination = destination ? strdup(destination) : NULL;
    flight->aircraft = aircraft ? strdup(aircraft) : NULL;
    flight->airline = airline ? strdup(airline) : NULL;
    flight->tracking_url = tracking_url ? strdup(tracking_url) : NULL;

    return flight;
}

// destruir
void flight_destroy(Flight *flight) {
    if (!flight) return;

    free(flight->id);
    free(flight->gate);
    free(flight->status);
    free(flight->origin);
    free(flight->destination);
    free(flight->aircraft);
    free(flight->airline);
    free(flight->tracking_url);

    free(flight);
}

// getters
const char *flight_get_id(const Flight *flight) { return flight ? flight->id : NULL; }

time_t flight_get_departure(const Flight *flight) { return flight ? flight->departure : 0; }

time_t flight_get_actual_departure(const Flight *flight) { return flight ? flight->actual_departure : 0; }

time_t flight_get_arrival(const Flight *flight) { return flight ? flight->arrival : 0; }

time_t flight_get_actual_arrival(const Flight *flight) { return flight ? flight->actual_arrival : 0; }

const char *flight_get_gate(const Flight *flight) { return flight ? flight->gate : NULL; }

const char *flight_get_status(const Flight *flight) { return flight ? flight->status : NULL; }

const char *flight_get_origin(const Flight *flight) { return flight ? flight->origin : NULL; }

const char *flight_get_destination(const Flight *flight) { return flight ? flight->destination : NULL; }

const char *flight_get_aircraft(const Flight *flight) { return flight ? flight->aircraft : NULL; }

const char *flight_get_airline(const Flight *flight) { return flight ? flight->airline : NULL; }

const char *flight_get_tracking_url(const Flight *flight) { return flight ? flight->tracking_url : NULL; }


