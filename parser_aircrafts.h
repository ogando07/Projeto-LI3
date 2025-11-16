#ifndef TRABALHO_PRATICO_PARSER_AIRCRAFTS_H
#define TRABALHO_PRATICO_PARSER_AIRCRAFTS_H

#include "database.h"
#include <stdio.h>

int parse_aircrafts(const char* filepath, Database* db, FILE* error_log);

#endif
