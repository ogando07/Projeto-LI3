#ifndef EXECUTOR_TESTES_H
#define EXECUTOR_TESTES_H

#include "metricas.h"
#include "comparador.h"

typedef struct TestConfig TestConfig;

TestConfig* create_test_config(const char* dataset_path, const char* input_file, 
                               const char* expected_results_path, const char* results_dir);

void free_test_config(TestConfig* config);

// Funções de acesso (Getters)
const char* get_test_config_dataset_path(const TestConfig* config);
const char* get_test_config_input_file(const TestConfig* config);
const char* get_test_config_expected_results_path(const TestConfig* config);

// Funções de acesso (Getters)
const char* get_test_config_results_dir(const TestConfig* config);

int execute_single_test(const TestConfig* config, const char* command, 
                       int query_num, ExecutionMetrics* metrics);

ProgramMetrics* execute_all_tests(const TestConfig* config);

ProgramMetrics* execute_all_tests_integrated(const TestConfig* config);

int validate_config(const TestConfig* config);

int ensure_results_directory(const char* results_dir);

int count_commands(const char* input_file);

#endif // EXECUTOR_TESTES_H