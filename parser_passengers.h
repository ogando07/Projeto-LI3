#ifndef TRABALHO_PRATICO_PARSER_PASSENGERS_H
#define TRABALHO_PRATICO_PARSER_PASSENGERS_H

#include "database.h"
#include <stdio.h>

int parse_passengers(const char* filepath, Database* db, FILE* error_log);

#endif
