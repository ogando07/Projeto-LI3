#ifndef TRABALHO_PRATICO_CONTROLLER_H
#define TRABALHO_PRATICO_CONTROLLER_H

#include "database.h"
#include <stdio.h>

typedef struct controller Controller;

// Lifecycle
Controller* controller_create(Database* db);
void controller_destroy(Controller* ctrl);

// Execute a single query line and write output to file
int controller_execute_query(Controller* ctrl, const char* query_line, FILE* output);

#endif
