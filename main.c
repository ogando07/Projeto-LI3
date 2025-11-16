#include "../include/database.h"
#include "../include/controller.h"
#include "../include/parser_airports.h"
#include "../include/parser_aircrafts.h"
#include "../include/parser_flights.h"
#include "../include/parser_passengers.h"
#include "../include/parser_reservations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <dataset_path> <input_file>\n", argv[0]);
        fprintf(stderr, "Example: %s dataset/ input.txt\n", argv[0]);
        return 1;
    }
    
    const char* dataset_path = argv[1];
    const char* input_file = argv[2];
    
    printf("=== Airport Management System - Phase 1 ===\n");
    printf("Dataset path: %s\n", dataset_path);
    printf("Input file: %s\n\n", input_file);
    
    // Create resultados directory if it doesn't exist
    mkdir("resultados", 0755);
    
    // Create database
    printf("Initializing database...\n");
    Database* db = database_create();
    if (!db) {
        fprintf(stderr, "Failed to create database\n");
        return 1;
    }
    
    // Build file paths
    char airports_path[512], aircrafts_path[512], passengers_path[512];
    char flights_path[512], reservations_path[512];
    
    snprintf(airports_path, sizeof(airports_path), "%s/airports.csv", dataset_path);
    snprintf(aircrafts_path, sizeof(aircrafts_path), "%s/aircrafts.csv", dataset_path);
    snprintf(passengers_path, sizeof(passengers_path), "%s/passengers.csv", dataset_path);
    snprintf(flights_path, sizeof(flights_path), "%s/flights.csv", dataset_path);
    snprintf(reservations_path, sizeof(reservations_path), "%s/reservations.csv", dataset_path);
    
    // Open error log files
    FILE* airports_errors = fopen("resultados/airports_errors.csv", "w");
    FILE* aircrafts_errors = fopen("resultados/aircrafts_errors.csv", "w");
    FILE* passengers_errors = fopen("resultados/passengers_errors.csv", "w");
    FILE* flights_errors = fopen("resultados/flights_errors.csv", "w");
    FILE* reservations_errors = fopen("resultados/reservations_errors.csv", "w");
    
    if (!airports_errors || !aircrafts_errors || !passengers_errors || 
        !flights_errors || !reservations_errors) {
        fprintf(stderr, "Failed to create error log files\n");
        database_destroy(db);
        return 1;
    }
    
    // Parse CSV files in order based on dependencies
    printf("\n=== Loading Data ===\n");
    
    printf("Loading airports...\n");
    if (parse_airports(airports_path, db, airports_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading airports\n");
    }
    
    printf("Loading aircrafts...\n");
    if (parse_aircrafts(aircrafts_path, db, aircrafts_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading aircrafts\n");
    }
    
    printf("Loading passengers...\n");
    if (parse_passengers(passengers_path, db, passengers_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading passengers\n");
    }
    
    printf("Loading flights...\n");
    if (parse_flights(flights_path, db, flights_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading flights\n");
    }
    
    printf("Loading reservations...\n");
    if (parse_reservations(reservations_path, db, reservations_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading reservations\n");
    }
    
    // Close error logs
    fclose(airports_errors);
    fclose(aircrafts_errors);
    fclose(passengers_errors);
    fclose(flights_errors);
    fclose(reservations_errors);
    
    printf("\n=== Data Loading Complete ===\n\n");
    
    // Create controller
    Controller* ctrl = controller_create(db);
    if (!ctrl) {
        fprintf(stderr, "Failed to create controller\n");
        database_destroy(db);
        return 1;
    }
    
    // Execute queries from input file
    printf("=== Executing Queries ===\n");
    FILE* input = fopen(input_file, "r");
    if (!input) {
        fprintf(stderr, "Failed to open input file: %s\n", input_file);
        controller_destroy(ctrl);
        database_destroy(db);
        return 1;
    }
    
    char query[256];
    int query_num = 1;
    
    while (fgets(query, sizeof(query), input)) {
        // Skip empty lines
        if (query[0] == '\n' || query[0] == '\0') continue;
        
        char output_path[256];
        snprintf(output_path, sizeof(output_path), 
                 "resultados/command%d_output.txt", query_num);
        
        FILE* output = fopen(output_path, "w");
        if (output) {
            printf("Executing Line %d: %s", query_num, query);
            controller_execute_query(ctrl, query, output);
            fclose(output);
        } else {
            fprintf(stderr, "Failed to create output file: %s\n", output_path);
        }
        
        query_num++;
    }
    
    fclose(input);
    
    printf("\n=== Done ===\n");
    printf("Executed %d queries.\n", query_num - 1);
    printf("Results written to resultados/ directory.\n");
    
    // Cleanup
    controller_destroy(ctrl);
    database_destroy(db);
    
    return 0;
}
