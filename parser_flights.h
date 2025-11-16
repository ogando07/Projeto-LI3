#ifndef TRABALHO_PRATICO_PARSER_FLIGHTS_H
#define TRABALHO_PRATICO_PARSER_FLIGHTS_H

#include "database.h"
#include <stdio.h>

int parse_flights(const char* filepath, Database* db, FILE* error_log);

#endif
