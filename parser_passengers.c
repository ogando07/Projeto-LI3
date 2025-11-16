#include "../include/parser_passengers.h"
#include "../include/parser_utils.h"
#include "../include/database.h"
#include "../include/passengers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_passengers(const char* filepath, Database* db, FILE* error_log) {
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
        char* fields[10];
        int field_count = parse_csv_line(line, fields, 10);
        
        if (field_count < 10) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        char* document_number = fields[0];
        char* first_name = fields[1];
        char* last_name = fields[2];
        char* dob_str = fields[3];
        char* nationality = fields[4];
        char* gender_str = fields[5];
        char* email = fields[6];
        char* phone = fields[7];
        char* address = fields[8];
        char* photo = fields[9];
        
        // Validate required fields
        if (is_empty_field(document_number) || is_empty_field(first_name) || 
            is_empty_field(last_name) || is_empty_field(dob_str) ||
            is_empty_field(nationality) || is_empty_field(gender_str)) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        // Validate formats
        if (!validate_document_number(document_number) ||
            !validate_date(dob_str) ||
            !validate_gender(gender_str) ||
            (email && !is_empty_field(email) && !validate_email(email))) {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
            continue;
        }
        
        time_t dob = parse_date(dob_str);
        char gender = gender_str[0];
        
        // Create passenger
        Passenger* passenger = passenger_create(
            document_number, first_name, last_name, dob, nationality, gender,
            email ? email : "", phone ? phone : "", 
            address ? address : "", photo ? photo : ""
        );
        
        if (passenger) {
            if (database_add_passenger(db, passenger) == 0) {
                valid_count++;
            } else {
                // Duplicate ID
                if (error_log) fprintf(error_log, "%s\n", original_line);
                passenger_destroy(passenger);
                error_count++;
            }
        } else {
            if (error_log) fprintf(error_log, "%s\n", original_line);
            error_count++;
        }
    }
    
    fclose(fp);
    printf("Passengers: %d valid, %d errors\n", valid_count, error_count);
    return 0;
}
