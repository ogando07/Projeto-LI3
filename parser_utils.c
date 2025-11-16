#include "../include/parser_utils.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// Parse a CSV line with quoted fields separated by commas
// Returns number of fields parsed, or -1 on error
int parse_csv_line(char* line, char** fields, int max_fields) {
    int field_count = 0;
    char* ptr = line;
    
    while (*ptr && field_count < max_fields) {
        // Skip leading whitespace
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        
        if (*ptr == '"') {
            // Quoted field
            ptr++; // Skip opening quote
            fields[field_count] = ptr;
            
            // Find closing quote
            while (*ptr && *ptr != '"') {
                ptr++;
            }
            
            if (*ptr == '"') {
                *ptr = '\0'; // Terminate the field
                ptr++; // Skip closing quote
            }
            
            // Skip comma and whitespace
            while (*ptr == ',' || *ptr == ' ' || *ptr == '\t') ptr++;
            
        } else if (*ptr == ',') {
            // Empty field
            fields[field_count] = ptr;
            *ptr = '\0';
            ptr++;
        } else if (*ptr == '\0' || *ptr == '\n' || *ptr == '\r') {
            // Empty field at end
            fields[field_count] = ptr;
            *ptr = '\0';
        } else {
            // Unquoted field (shouldn't happen in your format, but handle it)
            fields[field_count] = ptr;
            
            while (*ptr && *ptr != ',' && *ptr != '\n' && *ptr != '\r') {
                ptr++;
            }
            
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
        }
        
        field_count++;
    }
    
    return field_count;
}

// Trim leading and trailing whitespace
char* trim_whitespace(char* str) {
    if (!str) return NULL;
    
    // Trim leading
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == 0) return str;
    
    // Trim trailing
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    return str;
}

// Check if field is empty
bool is_empty_field(const char* field) {
    return !field || strlen(field) == 0;
}

// Validate IATA airport code (3 uppercase letters)
bool validate_airport_code(const char* code) {
    if (!code || strlen(code) != 3) return false;
    for (int i = 0; i < 3; i++) {
        if (!isupper((unsigned char)code[i])) return false;
    }
    return true;
}

// Validate latitude (-90 to 90)
bool validate_latitude(const char* lat_str) {
    if (!lat_str) return false;
    char* endptr;
    double lat = strtod(lat_str, &endptr);
    if (*endptr != '\0' && *endptr != '\n') return false;
    return lat >= -90.0 && lat <= 90.0;
}

// Validate longitude (-180 to 180)
bool validate_longitude(const char* lon_str) {
    if (!lon_str) return false;
    char* endptr;
    double lon = strtod(lon_str, &endptr);
    if (*endptr != '\0' && *endptr != '\n') return false;
    return lon >= -180.0 && lon <= 180.0;
}

// Validate airport type
bool validate_airport_type(const char* type) {
    if (!type) return false;
    return strcmp(type, "small_airport") == 0 ||
           strcmp(type, "medium_airport") == 0 ||
           strcmp(type, "large_airport") == 0 ||
           strcmp(type, "heliport") == 0 ||
           strcmp(type, "seaplane_base") == 0;
}

// Validate email format (username@domain.tld)
bool validate_email(const char* email) {
    if (!email) return false;
    
    const char* at = strchr(email, '@');
    if (!at || at == email) return false;
    
    const char* dot = strchr(at, '.');
    if (!dot || dot == at + 1 || dot[1] == '\0') return false;
    
    // Check that rstring (after last dot) is 2-3 characters
    const char* last_dot = strrchr(at, '.');
    size_t rstring_len = strlen(last_dot + 1);
    if (rstring_len < 2 || rstring_len > 3) return false;
    
    // Check username part [a-z0-9.]
    for (const char* p = email; p < at; p++) {
        if (!islower((unsigned char)*p) && !isdigit((unsigned char)*p) && *p != '.') {
            return false;
        }
    }
    
    // Check domain part [a-z] only (no digits in spec)
    for (const char* p = at + 1; *p; p++) {
        if (!islower((unsigned char)*p) && *p != '.') {
            return false;
        }
    }
    
    return true;
}

// Validate gender (M, F, or O)
bool validate_gender(const char* gender_str) {
    if (!gender_str || strlen(gender_str) != 1) return false;
    char g = gender_str[0];
    return g == 'M' || g == 'F' || g == 'O';
}

// Validate flight ID (2 letters + 5 digits, e.g., KS07323) - strict as per spec
bool validate_flight_id(const char* id) {
    if (!id || strlen(id) != 7) return false;
    if (!isupper((unsigned char)id[0]) || !isupper((unsigned char)id[1])) return false;
    for (int i = 2; i < 7; i++) {
        if (!isdigit((unsigned char)id[i])) return false;
    }
    return true;
}

// Validate reservation ID (R + 9 digits)
bool validate_reservation_id(const char* id) {
    if (!id || strlen(id) != 10) return false;
    if (id[0] != 'R') return false;
    for (int i = 1; i < 10; i++) {
        if (!isdigit((unsigned char)id[i])) return false;
    }
    return true;
}

// Validate document number (9 digits)
bool validate_document_number(const char* doc) {
    if (!doc || strlen(doc) != 9) return false;
    for (int i = 0; i < 9; i++) {
        if (!isdigit((unsigned char)doc[i])) return false;
    }
    return true;
}

// Validate date format (aaaa-mm-dd) - STRICT: only hyphens, no future dates
bool validate_date(const char* date_str) {
    if (!date_str || strlen(date_str) != 10) return false;
    
    // Must use hyphens, not slashes
    if (date_str[4] != '-' || date_str[7] != '-') {
        return false;
    }
    
    // Check all other characters are digits
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date_str[i])) return false;
    }
    
    int year, month, day;
    if (sscanf(date_str, "%d-%d-%d", &year, &month, &day) != 3) {
        return false;
    }
    
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    // Check date is not in the future
    time_t now = time(NULL);
    struct tm* current = localtime(&now);
    int current_year = current->tm_year + 1900;
    int current_month = current->tm_mon + 1;
    int current_day = current->tm_mday;
    
    if (year > current_year) return false;
    if (year == current_year && month > current_month) return false;
    if (year == current_year && month == current_month && day > current_day) return false;
    
    return true;
}

// Validate datetime format (aaaa-mm-dd hh:mm) - STRICT: only hyphens allowed
bool validate_datetime(const char* datetime_str) {
    if (!datetime_str || strlen(datetime_str) != 16) return false;
    
    // Check format strictly: aaaa-mm-dd hh:mm (only hyphens allowed)
    if (datetime_str[4] != '-' || datetime_str[7] != '-' || 
        datetime_str[10] != ' ' || datetime_str[13] != ':') {
        return false;
    }
    
    // Check all digit positions
    for (int i = 0; i < 16; i++) {
        if (i == 4 || i == 7 || i == 10 || i == 13) continue;
        if (!isdigit((unsigned char)datetime_str[i])) return false;
    }
    
    int year, month, day, hour, minute;
    if (sscanf(datetime_str, "%d-%d-%d %d:%d", 
               &year, &month, &day, &hour, &minute) != 5) {
        return false;
    }
    
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if (hour < 0 || hour > 23) return false;
    if (minute < 0 || minute > 59) return false;
    
    // For flights, we don't reject future dates as flights are scheduled in advance
    // Only reject dates that are clearly invalid (e.g., year < 1900 or > 2100)
    if (year < 1900 || year > 2100) return false;
    
    return true;
}

// Parse date string to time_t
time_t parse_date(const char* date_str) {
    struct tm tm = {0};
    sscanf(date_str, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    return mktime(&tm);
}

// Parse datetime string to time_t
time_t parse_datetime(const char* datetime_str) {
    struct tm tm = {0};
    sscanf(datetime_str, "%d-%d-%d %d:%d", 
           &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    return mktime(&tm);
}
