#ifndef COMPARADOR_H
#define COMPARADOR_H

#include <stdio.h>

typedef struct CompareResult CompareResult;

void free_compare_result(CompareResult* result);

CompareResult* compare_files(const char* expected_file, const char* actual_file);

int get_query_type(const char* command);

void print_compare_result(const CompareResult* result, int query_num, const char* actual_file);

int get_compare_result_files_match(const CompareResult* result);

#endif // COMPARADOR_H