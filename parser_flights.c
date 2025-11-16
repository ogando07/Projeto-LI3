#include "../include/parser_flights.h"
#include "../include/parser_utils.h"
#include "../include/database.h"
#include "../include/flights.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_flights(const char* filepath, Database* db, FILE* error_log) {
    FILE* fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open %s\n", filepath);
        return -1;
    }
    
    char line[2048];
    char original_line[2048];
    
    // Write header to error log
    if (error_log && fgets(line, sizeof(line), fp)) {
        fprintf(error_log, "%s", line);
    } else {
        fclose(fp);
        return -1;
    }
    
    int valid_count = 0;
    int error_count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        strcpy(original_line, line);
        line[strcspn(line, "\n")] = 0;
        
        // Parse CSV line with quoted fields
        char* fields[12];
        int field_count = parse_csv_line(line, fields, 12);
        
        if (field_count < 12) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        char* id = fields[0];
        char* departure_str = fields[1];
        char* actual_departure_str = fields[2];
        char* arrival_str = fields[3];
        char* actual_arrival_str = fields[4];
        char* gate = fields[5];
        char* status = fields[6];
        char* origin = fields[7];
        char* destination = fields[8];
        char* aircraft = fields[9];
        char* airline = fields[10];
        char* tracking_url = fields[11];
        
        // Validate required fields
        if (is_empty_field(id) || is_empty_field(departure_str) || 
            is_empty_field(arrival_str) || is_empty_field(origin) ||
            is_empty_field(destination) || is_empty_field(aircraft)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Validate formats
        if (!validate_flight_id(id) ||
            !validate_datetime(departure_str) ||
            !validate_datetime(arrival_str) ||
            !validate_airport_code(origin) ||
            !validate_airport_code(destination)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // STATUS-SPECIFIC VALIDATION
        bool is_cancelled = (status && strcmp(status, "Cancelled") == 0);
        bool is_delayed = (status && strcmp(status, "Delayed") == 0);
        
        // If Cancelled: actual times must be "N/A"
        if (is_cancelled) {
            if ((actual_departure_str && strcmp(actual_departure_str, "N/A") != 0 && !is_empty_field(actual_departure_str)) ||
                (actual_arrival_str && strcmp(actual_arrival_str, "N/A") != 0 && !is_empty_field(actual_arrival_str))) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
        }
        
        // Validate actual times if present and not "N/A"
        bool has_actual_departure = !is_empty_field(actual_departure_str) && strcmp(actual_departure_str, "N/A") != 0;
        bool has_actual_arrival = !is_empty_field(actual_arrival_str) && strcmp(actual_arrival_str, "N/A") != 0;
        
        if (has_actual_departure && !validate_datetime(actual_departure_str)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        if (has_actual_arrival && !validate_datetime(actual_arrival_str)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Parse times
        time_t departure = parse_datetime(departure_str);
        time_t arrival = parse_datetime(arrival_str);
        time_t actual_departure = has_actual_departure ? parse_datetime(actual_departure_str) : 0;
        time_t actual_arrival = has_actual_arrival ? parse_datetime(actual_arrival_str) : 0;
        
        // Logical validation: origin != destination
        if (strcmp(origin, destination) == 0) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Logical validation: arrival > departure
        if (arrival <= departure) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Logical validation: actual_arrival > actual_departure (if both exist)
        if (actual_departure > 0 && actual_arrival > 0 && actual_arrival <= actual_departure) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // If Delayed: actual times must be >= scheduled times
        if (is_delayed) {
            if (has_actual_departure && actual_departure < departure) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
            if (has_actual_arrival && actual_arrival < arrival) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
        }
        
        // Check if origin and destination airports exist
        if (!database_get_airport(db, origin) || !database_get_airport(db, destination)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Check if aircraft exists
        Aircraft* aircraft_obj = database_get_aircraft(db, aircraft);
        if (!aircraft_obj) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Create flight
        Flight* flight = flight_create(
            id, departure, actual_departure, arrival, actual_arrival,
            gate ? gate : "", status ? status : "", origin, destination,
            aircraft, airline ? airline : "", tracking_url ? tracking_url : ""
        );
        
        if (flight) {
            if (database_add_flight(db, flight) == 0) {
                valid_count++;
                // Increment aircraft flight count only for non-cancelled flights
                if (!is_cancelled) {
                    aircraft_increment_flight_count(aircraft_obj);
                }
                // Increment origin airport departure count (exclude cancelled)
                Airport* origin_airport = database_get_airport(db, origin);
                if (origin_airport && !is_cancelled) {
                    airport_increment_departures_count(origin_airport);
                }
            } else {
                // Duplicate ID
                if (error_log) fprintf(error_log, "%s\n", original_line);
                flight_destroy(flight);
                error_count++;
            }
        } else {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
        }
    }
    
    fclose(fp);
    printf("Flights: %d valid, %d errors\n", valid_count, error_count);
    return 0;
}
