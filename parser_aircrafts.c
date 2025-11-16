#include "../include/parser_aircrafts.h"
#include "../include/parser_utils.h"
#include "../include/database.h"
#include "../include/aircrafts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_aircrafts(const char* filepath, Database* db, FILE* error_log) {
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
        char* fields[6];
        int field_count = parse_csv_line(line, fields, 6);
        
        if (field_count < 6) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        char* id = fields[0];
        char* manufacturer = fields[1];
        char* model = fields[2];
        char* year_str = fields[3];
        char* capacity_str = fields[4];
        char* range_str = fields[5];
        
        // Validate required fields
        if (is_empty_field(id) || is_empty_field(manufacturer) || 
            is_empty_field(model) || is_empty_field(year_str) ||
            is_empty_field(capacity_str) || is_empty_field(range_str)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Parse numeric fields
        int year = atoi(year_str);
        int capacity = atoi(capacity_str);
        int range = atoi(range_str);
        
        // Validate ranges
        if (year < 1900 || year > 2025 || capacity <= 0 || range <= 0) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Create aircraft
        Aircraft* aircraft = aircraft_create(id, manufacturer, model, 
                                            year, capacity, range);
        if (aircraft) {
            if (database_add_aircraft(db, aircraft) == 0) {
                valid_count++;
            } else {
                // Duplicate ID
                if (error_log) fprintf(error_log, "%s\n", original_line);
                aircraft_destroy(aircraft);
                error_count++;
            }
        } else {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
        }
    }
    
    fclose(fp);
    printf("Aircrafts: %d valid, %d errors\n", valid_count, error_count);
    return 0;
}
