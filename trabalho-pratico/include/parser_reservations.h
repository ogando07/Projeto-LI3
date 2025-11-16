#ifndef TRABALHO_PRATICO_PARSER_RESERVATIONS_H
#define TRABALHO_PRATICO_PARSER_RESERVATIONS_H

#include "database.h"
#include <stdio.h>

int parse_reservations(const char* filepath, Database* db, FILE* error_log);

#endif
