#include "../include/airports.h"
#include <stdlib.h>
#include <string.h>

typedef struct airport {
    char *code;          // código IATA do aeroporto
    char *name;          // nome do aeroporto
    char *city;          // cidade onde o aeroporto se encontra localizado
    char *country;       // país onde o aeroporto se encontra localizado
    double latitude;     // latitude do aeroporto em graus decimais
    double longitude;    // longitude do aeroporto em graus decimais
    char *icao;          // código ICAO do aeroporto
    char *type;          // tipo do aeroporto
    size_t departures_count;  // contador auxiliar para Q3
} Airport;

// criar
Airport *airport_create(const char *code, const char *name, const char *city, const char *country, double latitude, double longitude, const char *icao, const char *type) {
    if (!code || strlen(code) == 0) return NULL;

    Airport *airport = malloc(sizeof(Airport));
    if (!airport) return NULL;

    airport->code = strdup(code);
    airport->name = name ? strdup(name) : NULL;
    airport->city = city ? strdup(city) : NULL;
    airport->country = country ? strdup(country) : NULL;
    airport->latitude = latitude;
    airport->longitude = longitude;
    airport->icao = icao ? strdup(icao) : NULL;
    airport->type = type ? strdup(type) : NULL;
    airport->departures_count = 0;

    return airport;
}

// destruir
void airport_destroy(Airport *airport) {
    if (!airport) return;

    free(airport->code);
    free(airport->name);
    free(airport->city);
    free(airport->country);
    free(airport->icao);
    free(airport->type);
    free(airport);
}

// getters
const char *airport_get_code(Airport *a) { return a ? a->code : NULL; }

const char *airport_get_name(Airport *a) { return a ? a->name : NULL; }

const char *airport_get_city(Airport *a) { return a ? a->city : NULL; }

const char *airport_get_country(Airport *a) { return a ? a->country : NULL; }

double airport_get_latitude(Airport *a) { return a ? a->latitude : 0.0; }

double airport_get_longitude(Airport *a) { return a ? a->longitude : 0.0; }

const char *airport_get_icao(Airport *a) { return a ? a->icao : NULL; }

const char *airport_get_type(Airport *a) { return a ? a->type : NULL; }

// Utility functions for departures count
size_t airport_get_departures_count(const Airport *a) {
    return a ? a->departures_count : 0;
}

void airport_increment_departures_count(Airport *a) {
    if (a) a->departures_count++;
}
