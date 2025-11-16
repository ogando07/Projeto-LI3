#ifndef TRABALHO_PRATICO_PARSER_AIRPORTS_H
#define TRABALHO_PRATICO_PARSER_AIRPORTS_H

#include "database.h"
#include <stdio.h>

int parse_airports(const char* filepath, Database* db, FILE* error_log);

#endif
