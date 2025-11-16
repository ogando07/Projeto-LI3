#ifndef METRICAS_H
#define METRICAS_H

#include <time.h>

// Estrutura simples de tempo usando apenas time.h
typedef struct SimpleTimer SimpleTimer;

typedef struct ExecutionMetrics ExecutionMetrics;

typedef struct QueryStats QueryStats;

typedef struct ProgramMetrics ProgramMetrics;

SimpleTimer* create_simple_timer(void);
void free_simple_timer(SimpleTimer* timer);
void simple_timer_start(SimpleTimer* timer);

double simple_timer_end(SimpleTimer* timer);

long estimate_memory_usage(void);

// Manter compatibilidade com funções antigas
int start_timing(struct timespec* start);
double end_timing(const struct timespec* start, struct timespec* end);
long get_memory_usage(void);

ProgramMetrics* init_program_metrics(int max_query_types);

void add_execution_result(ProgramMetrics* metrics, int query_type, 
                         double execution_time, int test_passed);

void finalize_metrics(ProgramMetrics* metrics);

void print_metrics_report(const ProgramMetrics* metrics);

double get_program_metrics_total_time(const ProgramMetrics* metrics);

void set_program_metrics_total_time(ProgramMetrics* metrics, double total_time);

void free_program_metrics(ProgramMetrics* metrics);

#endif // METRICAS_H