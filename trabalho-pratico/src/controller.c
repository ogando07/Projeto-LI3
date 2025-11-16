#include "../include/controller.h"
#include "../include/database.h"
#include "../include/airports.h"
#include "../include/aircrafts.h"
#include "../include/flights.h"
#include "../include/parser_utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct controller {
    Database* db;
} Controller;

// Forward declarations for query handlers
static void execute_query1(Controller* ctrl, const char* arg, FILE* output);
static void execute_query2(Controller* ctrl, const char* arg, FILE* output);
static void execute_query3(Controller* ctrl, const char* arg, FILE* output);

Controller* controller_create(Database* db) {
    Controller* ctrl = malloc(sizeof(Controller));
    if (!ctrl) return NULL;
    ctrl->db = db;
    return ctrl;
}

void controller_destroy(Controller* ctrl) {
    if (ctrl) free(ctrl);
}

int controller_execute_query(Controller* ctrl, const char* query_line, FILE* output) {
    if (!ctrl || !query_line || !output) return -1;
    
    char line[256];
    strncpy(line, query_line, sizeof(line) - 1);
    line[sizeof(line) - 1] = '\0';
    line[strcspn(line, "\n")] = 0;
    
    // Parse query number and arguments
    char* query_num_str = strtok(line, " ");
    char* arg = strtok(NULL, "");
    
    if (!query_num_str) return -1;
    
    int query_num = atoi(query_num_str);
    
    switch (query_num) {
        case 1:
            execute_query1(ctrl, arg, output);
            break;
        case 2:
            execute_query2(ctrl, arg, output);
            break;
        case 3:
            execute_query3(ctrl, arg, output);
            break;
        default:
            fprintf(stderr, "Unknown query: %d\n", query_num);
            return -1;
    }
    
    return 0;
}

// Q1: Airport summary by code
static void execute_query1(Controller* ctrl, const char* code, FILE* output) {
    if (!code) {
        fprintf(output, "\n");
        return;
    }
    
    Airport* airport = database_get_airport(ctrl->db, code);
    if (!airport) {
        fprintf(output, "\n");
        return;
    }
    
    fprintf(output, "%s,%s,%s,%s,%s\n",
            airport_get_code(airport),
            airport_get_name(airport),
            airport_get_city(airport),
            airport_get_country(airport),
            airport_get_type(airport));
}

// Comparison function for sorting aircrafts by flight count (descending), then by ID
static int compare_aircrafts(const void* a, const void* b) {
    Aircraft* aircraft_a = *(Aircraft**)a;
    Aircraft* aircraft_b = *(Aircraft**)b;
    
    int count_a = aircraft_get_flight_count(aircraft_a);
    int count_b = aircraft_get_flight_count(aircraft_b);
    
    if (count_b != count_a) {
        return count_b - count_a; // Descending by count
    }
    
    // If counts are equal, sort by ID ascending
    return strcmp(aircraft_get_id(aircraft_a), aircraft_get_id(aircraft_b));
}

// Q2: Top N aircrafts by flight count, optionally filtered by manufacturer
static void execute_query2(Controller* ctrl, const char* arg, FILE* output) {
    if (!arg) return;
    
    int n;
    char manufacturer[128] = {0};
    
    // Parse arguments
    if (strchr(arg, ' ')) {
        sscanf(arg, "%d %127s", &n, manufacturer);
    } else {
        n = atoi(arg);
    }
    
     if (n <= 0) return;
     /* Use an unsigned size_t for comparisons with filtered_count to avoid
         signed/unsigned warnings and incorrect behavior when mixing types. */
     size_t requested = (size_t)n;
    
    // Get all aircrafts
    size_t count;
    Aircraft** all_aircrafts = database_get_all_aircrafts(ctrl->db, &count);
    if (!all_aircrafts || count == 0) {
        // No aircrafts in database
        fprintf(output, "\n");
        return;
    }
    
    // Filter by manufacturer if specified
    Aircraft** filtered = NULL;
    size_t filtered_count = 0;
    
    if (strlen(manufacturer) > 0) {
        filtered = malloc(count * sizeof(Aircraft*));
        for (size_t i = 0; i < count; i++) {
            if (strcmp(aircraft_get_manufacturer(all_aircrafts[i]), manufacturer) == 0) {
                filtered[filtered_count++] = all_aircrafts[i];
            }
        }
    } else {
        filtered = all_aircrafts;
        filtered_count = count;
    }
    
    if (filtered_count == 0) {
        if (strlen(manufacturer) > 0) free(filtered);
        // No aircrafts match the filter / none available
        fprintf(output, "\n");
        return;
    }
    
    // Sort by flight count (descending), then by ID (ascending)
    qsort(filtered, filtered_count, sizeof(Aircraft*), compare_aircrafts);
    
    // Output top N
    size_t output_count = requested < filtered_count ? requested : filtered_count;
    for (size_t i = 0; i < output_count; i++) {
        fprintf(output, "%s,%s,%s,%d\n",
                aircraft_get_id(filtered[i]),
                aircraft_get_manufacturer(filtered[i]),
                aircraft_get_model(filtered[i]),
                aircraft_get_flight_count(filtered[i]));
    }
    
    if (strlen(manufacturer) > 0) free(filtered);
}

// Helper structure for Q3
typedef struct {
    Airport* airport;
    size_t departure_count;
} AirportDepartureCount;

// Comparison function for sorting airports by departure count (descending), then by code
static int compare_airport_departures(const void* a, const void* b) {
    AirportDepartureCount* adc_a = (AirportDepartureCount*)a;
    AirportDepartureCount* adc_b = (AirportDepartureCount*)b;
    
    if (adc_b->departure_count != adc_a->departure_count) {
        return adc_b->departure_count - adc_a->departure_count; // Descending
    }
    
    // If counts are equal, sort by code ascending (lexicographically)
    return strcmp(airport_get_code(adc_a->airport), airport_get_code(adc_b->airport));
}

// Q3: Airport with most departures between two dates
static void execute_query3(Controller* ctrl, const char* arg, FILE* output) {
    if (!arg) return;
    
    char date1_str[11], date2_str[11];
    if (sscanf(arg, "%10s %10s", date1_str, date2_str) != 2) return;
    
    char date1_dt[17]; // "YYYY-MM-DD hh:mm"
    char date2_dt[17];
    snprintf(date1_dt, sizeof(date1_dt), "%s 00:00", date1_str);
    snprintf(date2_dt, sizeof(date2_dt), "%s 00:00", date2_str);

    if (!validate_datetime(date1_dt) || !validate_datetime(date2_dt)) return;

    time_t date1 = parse_date(date1_str);
    time_t date2 = parse_date(date2_str);
    
    // Ensure date1 <= date2
    if (date1 > date2) {
        time_t temp = date1;
        date1 = date2;
        date2 = temp;
    }
    
    // Make date2 end of day (23:59:59)
    date2 += 86399; // Add 23:59:59
    
    // Count departures per airport
    size_t airport_count;
    Airport** airports = database_get_all_airports(ctrl->db, &airport_count);
    
    AirportDepartureCount* counts = calloc(airport_count, sizeof(AirportDepartureCount));
    for (size_t i = 0; i < airport_count; i++) {
        counts[i].airport = airports[i];
        counts[i].departure_count = 0;
    }
    
    // Count flights for each airport in the date range
    size_t flight_count;
    Flight** flights = database_get_all_flights(ctrl->db, &flight_count);
    
    for (size_t i = 0; i < flight_count; i++) {
        Flight* flight = flights[i];
        const char* status = flight_get_status(flight);
        
        // Skip cancelled flights
        if (status && strcmp(status, "Cancelled") == 0) continue;
        
        // Get actual departure time, or scheduled if not available
        time_t departure = flight_get_actual_departure(flight);
        if (departure == 0) {
            departure = flight_get_departure(flight);
        }
        
        // Check if in range
        if (departure >= date1 && departure <= date2) {
            const char* origin = flight_get_origin(flight);
            
            // Find the airport and increment count
            for (size_t j = 0; j < airport_count; j++) {
                if (strcmp(airport_get_code(counts[j].airport), origin) == 0) {
                    counts[j].departure_count++;
                    break;
                }
            }
        }
    }
    
    // Sort by departure count (descending), then by code (ascending)
    qsort(counts, airport_count, sizeof(AirportDepartureCount), compare_airport_departures);
    
    // If there are no airports in the database
    if (airport_count == 0) {
        fprintf(output, "\n");
        free(counts);
        return;
    }

    // Output the top airport (if it has departures)
    if (counts[0].departure_count > 0) {
        fprintf(output, "%s,%s,%s,%s,%lu\n",
                airport_get_code(counts[0].airport),
                airport_get_name(counts[0].airport),
                airport_get_city(counts[0].airport),
                airport_get_country(counts[0].airport),
                (unsigned long)counts[0].departure_count);
    } else {
        // No airport has departures in the given timeframe
        fprintf(output, "\n");
    }

    free(counts);
}
