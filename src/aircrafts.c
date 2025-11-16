#include "../include/aircrafts.h"
#include <stdlib.h>
#include <string.h>

typedef struct aircraft {
    char *id;    		 // identificador único da aeronave, i.e., tail number
    char *manufacturer;  // fabricante da aeronave
    char *model;         // modelo da aeronave
    int year;            // ano de fabricação da aeronave
    int capacity;        // capacidade máxima de passageiros da aeronave
    int range;           // alcance máximo da aeronave em km
    int flight_count;    // contador auxiliar para Q2 e Q3 (se contagens são usadas com frequência e não mudam depois do parsing)
} Aircraft;

// cria
Aircraft *aircraft_create(const char *id, const char *manufacturer, const char *model, int year, int capacity, int range) {
    if (!id || strlen(id) == 0) return NULL;

    Aircraft *aircraft = malloc(sizeof(Aircraft));
    if (!aircraft) return NULL;

    aircraft->id = strdup(id);
    aircraft->manufacturer = manufacturer ? strdup(manufacturer) : NULL;
    aircraft->model = model ? strdup(model) : NULL;
    aircraft->year = year;
    aircraft->capacity = capacity;
    aircraft->range = range;
    aircraft->flight_count = 0;

    return aircraft;
}

// destroi
void aircraft_destroy(Aircraft *aircraft) {
    if (!aircraft) return;

    free(aircraft->id);
    free(aircraft->manufacturer);
    free(aircraft->model);
    free(aircraft);
}

// getters
const char *aircraft_get_id(const Aircraft *aircraft) { return aircraft ? aircraft->id : NULL; }

const char *aircraft_get_manufacturer(const Aircraft *aircraft) { return aircraft ? aircraft->manufacturer : NULL; }

const char *aircraft_get_model(const Aircraft *aircraft) { return aircraft ? aircraft->model : NULL; }

int aircraft_get_year(const Aircraft *aircraft) { return aircraft ? aircraft->year : 0; }

int aircraft_get_capacity(const Aircraft *aircraft) { return aircraft ? aircraft->capacity : 0; }

int aircraft_get_range(const Aircraft *aircraft) { return aircraft ? aircraft->range : 0; }

int aircraft_get_flight_count(const Aircraft *aircraft) { return aircraft ? aircraft->flight_count : 0; }


// utility function
void aircraft_increment_flight_count(Aircraft *aircraft) {
    if (aircraft) aircraft->flight_count++;
}
