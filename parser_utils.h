#ifndef TRABALHO_PRATICO_PARSER_UTILS_H
#define TRABALHO_PRATICO_PARSER_UTILS_H

#include <stdbool.h>
#include <time.h>

// CSV parsing for quoted fields
int parse_csv_line(char* line, char** fields, int max_fields);

// Date/time validation and parsing
bool validate_date(const char* date_str);
bool validate_datetime(const char* datetime_str);
time_t parse_date(const char* date_str);
time_t parse_datetime(const char* datetime_str);

// Field validation
bool validate_airport_code(const char* code);
bool validate_latitude(const char* lat_str);
bool validate_longitude(const char* lon_str);
bool validate_airport_type(const char* type);
bool validate_email(const char* email);
bool validate_gender(const char* gender_str);
bool validate_flight_id(const char* id);
bool validate_reservation_id(const char* id);
bool validate_document_number(const char* doc);

// Utility functions
char* trim_whitespace(char* str);
bool is_empty_field(const char* field);

#endif
