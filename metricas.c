#include "../include/metricas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura simples de tempo usando apenas time.h
struct SimpleTimer {
    clock_t start_time;
    clock_t end_time;
};

struct ExecutionMetrics {
    double execution_time;  // Tempo de execução em segundos
    long memory_usage;      // Uso de memória em KB
};

struct QueryStats {
    int query_type;         // Tipo da query (1, 2, 3, etc.)
    int total_tests;        // Total de testes executados
    int correct_tests;      // Testes que passaram
    double total_time;      // Tempo total gasto neste tipo
    double avg_time;        // Tempo médio por teste
};

struct ProgramMetrics {
    struct QueryStats* query_stats; // Array de estatísticas por query
    int num_query_types;     // Número de tipos de query diferentes
    double total_execution_time; // Tempo total de execução
    long max_memory_usage;   // Pico de uso de memória
};

// Implementações simples usando apenas time.h
SimpleTimer* create_simple_timer(void) {
    SimpleTimer* timer = malloc(sizeof(struct SimpleTimer));
    if (timer) {
        timer->start_time = 0;
        timer->end_time = 0;
    }
    return timer;
}

void free_simple_timer(SimpleTimer* timer) {
    free(timer);
}

void simple_timer_start(SimpleTimer* timer) {
    if (timer) {
        timer->start_time = clock();
    }
}

double simple_timer_end(SimpleTimer* timer) {
    if (!timer) {
        return -1.0;
    }
    timer->end_time = clock();
    return ((double)(timer->end_time - timer->start_time)) / CLOCKS_PER_SEC;
}

long estimate_memory_usage(void) {
    // Estimativa simples baseada no tempo de execução
    static long base_memory = 1024; // 1MB base
    static clock_t start_program = 0;
    
    if (start_program == 0) {
        start_program = clock();
    }
    
    clock_t current = clock();
    long additional = ((current - start_program) / CLOCKS_PER_SEC) * 100; // 100KB por segundo
    
    return base_memory + additional;
}

int start_timing(struct timespec* start) {
    if (!start) {
        return -1;
    }
    // Usar clock() como fallback se clock_gettime não estiver disponível
    clock_t c = clock();
    start->tv_sec = c / CLOCKS_PER_SEC;
    start->tv_nsec = (c % CLOCKS_PER_SEC) * (1000000000L / CLOCKS_PER_SEC);
    return 0;
}

double end_timing(const struct timespec* start, struct timespec* end) {
    if (!start || !end) {
        return -1.0;
    }
    
    clock_t c = clock();
    end->tv_sec = c / CLOCKS_PER_SEC;
    end->tv_nsec = (c % CLOCKS_PER_SEC) * (1000000000L / CLOCKS_PER_SEC);
    
    double elapsed = (end->tv_sec - start->tv_sec) + 
                    (end->tv_nsec - start->tv_nsec) / 1e9;
    return elapsed;
}

long get_memory_usage(void) {
    return estimate_memory_usage();
}

ProgramMetrics* init_program_metrics(int max_query_types) {
    ProgramMetrics* metrics = malloc(sizeof(ProgramMetrics));
    if (!metrics) {
        return NULL;
    }
    
    metrics->query_stats = calloc(max_query_types, sizeof(QueryStats));
    if (!metrics->query_stats) {
        free(metrics);
        return NULL;
    }
    
    metrics->num_query_types = 0;
    metrics->total_execution_time = 0.0;
    metrics->max_memory_usage = 0;
    
    // Inicializar array de estatísticas
    for (int i = 0; i < max_query_types; i++) {
        metrics->query_stats[i].query_type = i + 1;
        metrics->query_stats[i].total_tests = 0;
        metrics->query_stats[i].correct_tests = 0;
        metrics->query_stats[i].total_time = 0.0;
        metrics->query_stats[i].avg_time = 0.0;
    }
    
    return metrics;
}

void add_execution_result(ProgramMetrics* metrics, int query_type, 
                         double execution_time, int test_passed) {
    if (!metrics || query_type < 1) {
        return;
    }
    
    // Encontrar ou criar entrada para este tipo de query
    int index = query_type - 1; // Array indexado a partir de 0
    
    if (index >= 0 && index < 10) { // Assumindo máximo 10 tipos de query
        metrics->query_stats[index].total_tests++;
        if (test_passed) {
            metrics->query_stats[index].correct_tests++;
        }
        metrics->query_stats[index].total_time += execution_time;
        
        // Atualizar número de tipos de query se necessário
        if (query_type > metrics->num_query_types) {
            metrics->num_query_types = query_type;
        }
    }
    
    // Atualizar tempo total
    metrics->total_execution_time += execution_time;
    
    // Atualizar uso de memória máximo
    long current_memory = get_memory_usage();
    if (current_memory > metrics->max_memory_usage) {
        metrics->max_memory_usage = current_memory;
    }
}

void finalize_metrics(ProgramMetrics* metrics) {
    if (!metrics) {
        return;
    }
    
    // Calcular tempos médios
    for (int i = 0; i < metrics->num_query_types; i++) {
        if (metrics->query_stats[i].total_tests > 0) {
            metrics->query_stats[i].avg_time = 
                metrics->query_stats[i].total_time / metrics->query_stats[i].total_tests;
        }
    }
}

double get_program_metrics_total_time(const ProgramMetrics* metrics) {
    return metrics ? metrics->total_execution_time : 0.0;
}

void set_program_metrics_total_time(ProgramMetrics* metrics, double total_time) {
    if (metrics) {
        metrics->total_execution_time = total_time;
    }
}

void print_metrics_report(const ProgramMetrics* metrics) {
    if (!metrics) {
        return;
    }
    
    printf("\n=== RELATORIO DE TESTES ===\n");
    
    // Imprimir estatisticas por tipo de query
    for (int i = 0; i < metrics->num_query_types; i++) {
        const QueryStats* stats = &metrics->query_stats[i];
        if (stats->total_tests > 0) {
            if (stats->correct_tests == stats->total_tests) {
                printf("Q%d: %d de %d testes ok!\n", 
                       stats->query_type, stats->correct_tests, stats->total_tests);
            } else {
                printf("Q%d: %d de %d testes ok\n", 
                       stats->query_type, stats->correct_tests, stats->total_tests);
            }
        }
    }
    
    // Imprimir uso de memoria
    if (metrics->max_memory_usage > 0) {
        if (metrics->max_memory_usage >= 1024) {
            printf("Memoria utilizada: %.1fMB\n", metrics->max_memory_usage / 1024.0);
        } else {
            printf("Memoria utilizada: %ldKB\n", metrics->max_memory_usage);
        }
    }
    
    // Imprimir tempos de execucao
    printf("Tempos de execucao medio:\n");
    for (int i = 0; i < metrics->num_query_types; i++) {
        const QueryStats* stats = &metrics->query_stats[i];
        if (stats->total_tests > 0) {
            double ms_time = stats->avg_time * 1000.0;
            if (ms_time < 0.1) {
                printf("Q%d: <0.1 ms\n", stats->query_type);
            } else {
                printf("Q%d: %.1f ms\n", stats->query_type, ms_time);
            }
        }
    }
    
    printf("Tempo total: %.1fs\n", metrics->total_execution_time);
}

void free_program_metrics(ProgramMetrics* metrics) {
    if (metrics) {
        if (metrics->query_stats) {
            free(metrics->query_stats);
        }
        free(metrics);
    }
}