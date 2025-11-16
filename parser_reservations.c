#include "../include/parser_reservations.h"
#include "../include/parser_utils.h"
#include "../include/database.h"
#include "../include/reservations.h"
#include "../include/flights.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int parse_reservations(const char* filepath, Database* db, FILE* error_log) {
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
        char* fields[8];
        int field_count = parse_csv_line(line, fields, 8);
        
        if (field_count < 8) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        char* id = fields[0];
        char* flight_ids_str = fields[1];
        char* document_number = fields[2];
        char* seat = fields[3];
        char* price_str = fields[4];
        char* extra_luggage_str = fields[5];
        char* priority_boarding_str = fields[6];
        char* qr_code = fields[7];
        
        // Validate required fields
        if (is_empty_field(id) || is_empty_field(flight_ids_str) || 
            is_empty_field(document_number) || is_empty_field(price_str)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Validate reservation ID
        if (!validate_reservation_id(id)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Validate document number
        if (!validate_document_number(document_number)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Check if passenger exists
        if (!database_get_passenger(db, document_number)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Parse flight IDs (can be 1 or 2, must be in [list] format if multiple)
        char* flight_ids[2] = {NULL, NULL};
        size_t flight_count = 0;
        
        // Check if it's a list (starts with [ and ends with ])
        bool is_list = (flight_ids_str[0] == '[');
        
        if (is_list) {
            // Validate list format
            size_t len = strlen(flight_ids_str);
            if (len < 2 || flight_ids_str[len-1] != ']') {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
            
            // Remove [ and ]
            char* flight_list = flight_ids_str + 1;
            flight_list[len-2] = '\0';
            
            // Split by comma
            char* flight_id = strtok(flight_list, ",");
            while (flight_id && flight_count < 2) {
                // Trim whitespace
                while (*flight_id == ' ') flight_id++;
                char* end = flight_id + strlen(flight_id) - 1;
                while (end > flight_id && *end == ' ') *end-- = '\0';
                
                // Remove single quotes if present
                if (flight_id[0] == '\'' && end >= flight_id && *end == '\'') {
                    flight_id++;
                    *end = '\0';
                }
                
                if (!validate_flight_id(flight_id)) {
                    flight_count = 0;
                    break;
                }
                // Check if flight exists
                if (!database_get_flight(db, flight_id)) {
                    flight_count = 0;
                    break;
                }
                // Allocate and copy flight ID
                flight_ids[flight_count] = strdup(flight_id);
                if (!flight_ids[flight_count]) {
                    flight_count = 0;
                    break;
                }
                flight_count++;
                flight_id = strtok(NULL, ",");
            }
            
            if (flight_count == 0 || flight_count > 2) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
        } else {
            // Single flight (no brackets)
            if (!validate_flight_id(flight_ids_str)) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
            // Check if flight exists
            if (!database_get_flight(db, flight_ids_str)) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
            // Allocate and copy flight ID
            flight_ids[0] = strdup(flight_ids_str);
            if (!flight_ids[0]) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
            flight_count = 1;
        }
        
        // If 2 flights: validate connection (destination of first == origin of second)
        if (flight_count == 2) {
            Flight* flight1 = database_get_flight(db, flight_ids[0]);
            Flight* flight2 = database_get_flight(db, flight_ids[1]);
            
            if (!flight1 || !flight2) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
            
            const char* dest1 = flight_get_destination(flight1);
            const char* orig2 = flight_get_origin(flight2);
            
            if (strcmp(dest1, orig2) != 0) {
                if (error_log) fprintf(error_log, "%s\n", original_line);
                error_count++;
                continue;
            }
        }
        
        // Parse price
        double price = atof(price_str);
        if (price < 0) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Parse booleans
        bool extra_luggage = extra_luggage_str && 
                            (strcmp(extra_luggage_str, "true") == 0 || 
                             strcmp(extra_luggage_str, "1") == 0);
        bool priority_boarding = priority_boarding_str && 
                                (strcmp(priority_boarding_str, "true") == 0 || 
                                 strcmp(priority_boarding_str, "1") == 0);
        
        // Create reservation
        Reservation* reservation = reservation_create(
            id, (const char**)flight_ids, document_number,
            seat ? seat : "", price, extra_luggage, priority_boarding,
            qr_code ? qr_code : "", flight_count
        );
        
        if (reservation) {
            if (database_add_reservation(db, reservation) == 0) {
                valid_count++;
            } else {
                // Duplicate ID
                if (error_log) fprintf(error_log, "%s\n", original_line);
                reservation_destroy(reservation);
                error_count++;
            }
        } else {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
        }
    }
    
    fclose(fp);
    printf("Reservations: %d valid, %d errors\n", valid_count, error_count);
    return 0;
}
