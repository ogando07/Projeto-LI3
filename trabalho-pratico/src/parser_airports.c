#include "../include/parser_airports.h"
#include "../include/parser_utils.h"
#include "../include/database.h"
#include "../include/airports.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_airports(const char* filepath, Database* db, FILE* error_log) {
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
        
        char* code = fields[0];
        char* name = fields[1];
        char* city = fields[2];
        char* country = fields[3];
        char* latitude_str = fields[4];
        char* longitude_str = fields[5];
        char* icao = fields[6];
        char* type = fields[7];
        
        // Validate required fields
        if (is_empty_field(code) || is_empty_field(name) || 
            is_empty_field(city) || is_empty_field(country) ||
            is_empty_field(latitude_str) || is_empty_field(longitude_str) ||
            is_empty_field(type)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Validate formats
        if (!validate_airport_code(code) ||
            !validate_latitude(latitude_str) ||
            !validate_longitude(longitude_str) ||
            !validate_airport_type(type)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        double latitude = atof(latitude_str);
        double longitude = atof(longitude_str);
        
        // Create airport
        Airport* airport = airport_create(code, name, city, country, 
                                         latitude, longitude, 
                                         icao ? icao : "", type);
        if (airport) {
            if (database_add_airport(db, airport) == 0) {
                valid_count++;
            } else {
                // Duplicate ID
                if (error_log) fprintf(error_log, "%s\n", original_line);
                airport_destroy(airport);
                error_count++;
            }
        } else {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
        }
    }
    
    fclose(fp);
    printf("Airports: %d valid, %d errors\n", valid_count, error_count);
    return 0;
}
