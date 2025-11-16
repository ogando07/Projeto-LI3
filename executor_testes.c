#include "../include/executor_testes.h"
#include "../include/comparador.h" 
#include "../include/metricas.h"
#include "../include/database.h"
#include "../include/controller.h"
#include "../include/parser_airports.h"
#include "../include/parser_aircrafts.h"
#include "../include/parser_flights.h"
#include "../include/parser_passengers.h"
#include "../include/parser_reservations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

struct TestConfig {
    const char* dataset_path;      // Caminho para o dataset
    const char* input_file;        // Ficheiro com comandos
    const char* expected_results_path; // Pasta com resultados esperados
    const char* results_dir;           // Diretório onde são gerados os resultados
};

TestConfig* create_test_config(const char* dataset_path, const char* input_file, 
                               const char* expected_results_path, const char* results_dir) {
    TestConfig* config = malloc(sizeof(struct TestConfig));
    if (!config) {
        return NULL;
    }
    
    config->dataset_path = dataset_path;
    config->input_file = input_file;
    config->expected_results_path = expected_results_path;
    config->results_dir = results_dir;
    
    return config;
}

void free_test_config(TestConfig* config) {
    free(config);
}

const char* get_test_config_dataset_path(const TestConfig* config) {
    return config ? config->dataset_path : NULL;
}

const char* get_test_config_input_file(const TestConfig* config) {
    return config ? config->input_file : NULL;
}

const char* get_test_config_expected_results_path(const TestConfig* config) {
    return config ? config->expected_results_path : NULL;
}

const char* get_test_config_results_dir(const TestConfig* config) {
    return config ? config->results_dir : NULL;
}

int validate_config(const TestConfig* config) {
    if (!config) {
        return 0;
    }
    
    // Verificar se ficheiro de input existe
    FILE* test_file = fopen(config->input_file, "r");
    if (!test_file) {
        fprintf(stderr, "Erro: Ficheiro de input '%s' nao encontrado\n", config->input_file);
        return 0;
    }
    fclose(test_file);
    
    // Verificar se um ficheiro esperado existe
    char expected_test[512];
    snprintf(expected_test, sizeof(expected_test), "%s/command1_output.txt", config->expected_results_path);
    test_file = fopen(expected_test, "r");
    if (!test_file) {
        fprintf(stderr, "Erro: Diretorio de resultados esperados '%s' nao encontrado\n", config->expected_results_path);
        return 0;
    }
    fclose(test_file);
    
    return 1;
}

int ensure_results_directory(const char* results_dir) {
    if (!results_dir) {
        return -1;
    }
    
    // Create resultados directory if it doesn't exist
    mkdir(results_dir, 0755);
    
    // Testar se consegue criar um ficheiro (verificação)
    char test_file[512];
    snprintf(test_file, sizeof(test_file), "%s/test_write.tmp", results_dir);
    FILE* test = fopen(test_file, "w");
    if (test) {
        fclose(test);
        remove(test_file);
        return 0; // Diretório criado/existe
    }
    
    printf("AVISO: Nao foi possivel criar/acessar diretorio '%s'\n", results_dir);
    return -1;
}

int count_commands(const char* input_file) {
    FILE* file = fopen(input_file, "r");
    if (!file) {
        return -1;
    }
    
    char line[256];
    int count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (line[0] != '\n' && line[0] != '\0') {
            count++;
        }
    }
    
    fclose(file);
    return count;
}

ProgramMetrics* execute_all_tests_integrated(const TestConfig* config) {
    if (!config || !validate_config(config)) {
        return NULL;
    }
    
    // Inicializar métricas
    ProgramMetrics* metrics = init_program_metrics(10);
    if (!metrics) {
        return NULL;
    }
    
    printf("=== CARREGANDO DATABASE ===\n");
    
    // Garantir que diretório de resultados existe
    ensure_results_directory(get_test_config_results_dir(config));
    
    // Medir tempo total de execução
    SimpleTimer* total_timer = create_simple_timer();
    if (!total_timer) {
        // Handle error
    }
    simple_timer_start(total_timer);
    
    // Criar database
    Database* db = database_create();
    if (!db) {
        fprintf(stderr, "Falha ao criar database\n");
        free_program_metrics(metrics);
        return NULL;
    }
    
    // Construir caminhos dos ficheiros
    char airports_path[512], aircrafts_path[512], passengers_path[512];
    char flights_path[512], reservations_path[512];
    
    snprintf(airports_path, sizeof(airports_path), "%s/airports.csv", config->dataset_path);
    snprintf(aircrafts_path, sizeof(aircrafts_path), "%s/aircrafts.csv", config->dataset_path);
    snprintf(passengers_path, sizeof(passengers_path), "%s/passengers.csv", config->dataset_path);
    snprintf(flights_path, sizeof(flights_path), "%s/flights.csv", config->dataset_path);
    snprintf(reservations_path, sizeof(reservations_path), "%s/reservations.csv", config->dataset_path);
    
    // Abrir ficheiros de erro (como o programa principal faz)
    FILE* airports_errors = fopen("resultados/airports_errors.csv", "w");
    FILE* aircrafts_errors = fopen("resultados/aircrafts_errors.csv", "w");
    FILE* passengers_errors = fopen("resultados/passengers_errors.csv", "w");
    FILE* flights_errors = fopen("resultados/flights_errors.csv", "w");
    FILE* reservations_errors = fopen("resultados/reservations_errors.csv", "w");
    
    if (!airports_errors || !aircrafts_errors || !passengers_errors || 
        !flights_errors || !reservations_errors) {
        fprintf(stderr, "Failed to create error log files\n");
        database_destroy(db);
        free_program_metrics(metrics);
        return NULL;
    }
    
    // Carregar dados (replicar exatamente o que o programa principal faz)
    printf("Carregando airports...\n");
    if (parse_airports(airports_path, db, airports_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading airports\n");
    }
    
    printf("Carregando aircrafts...\n");
    if (parse_aircrafts(aircrafts_path, db, aircrafts_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading aircrafts\n");
    }
    
    printf("Carregando passengers...\n");
    if (parse_passengers(passengers_path, db, passengers_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading passengers\n");
    }
    
    printf("Carregando flights...\n");
    if (parse_flights(flights_path, db, flights_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading flights\n");
    }
    
    printf("Carregando reservations...\n");
    if (parse_reservations(reservations_path, db, reservations_errors) != 0) {
        fprintf(stderr, "Warning: Issues loading reservations\n");
    }
    
    // Fechar ficheiros de erro
    fclose(airports_errors);
    fclose(aircrafts_errors);
    fclose(passengers_errors);
    fclose(flights_errors);
    fclose(reservations_errors);
    remove("errors_temp.txt"); // Limpar ficheiro temporário
    
    // Criar controller
    Controller* ctrl = controller_create(db);
    if (!ctrl) {
        fprintf(stderr, "Falha ao criar controller\n");
        database_destroy(db);
        free_program_metrics(metrics);
        return NULL;
    }
    
    printf("Database carregada!\n\n");
    printf("=== EXECUTANDO QUERIES ===\n");
    
    // Abrir ficheiro de input
    FILE* input_file = fopen(config->input_file, "r");
    if (!input_file) {
        controller_destroy(ctrl);
        database_destroy(db);
        free_program_metrics(metrics);
        return NULL;
    }
    
    char command[256];
    int query_num = 1;
    
    while (fgets(command, sizeof(command), input_file)) {
        // Remover quebra de linha
        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n') {
            command[len - 1] = '\0';
        }
        
        // Skip empty lines
        if (command[0] == '\0' || command[0] == '\n') {
            continue;
        }
        
        // Determinar tipo de query
        int query_type = get_query_type(command);
        if (query_type == 0) {
            query_num++;
            continue;
        }
        
        // Executar query individual
        char output_path[512];
        snprintf(output_path, sizeof(output_path), 
                 "%s/command%d_output.txt", config->results_dir, query_num);
        
        FILE* output = fopen(output_path, "w");
        if (!output) {
            fprintf(stderr, "Erro ao criar ficheiro: %s\n", output_path);
            query_num++;
            continue;
        }
        
        // Medir tempo para esta query
        SimpleTimer* query_timer = create_simple_timer();
        if (!query_timer) {
            // Handle error
        }
        simple_timer_start(query_timer);
        
        // Executar query
        int query_success = controller_execute_query(ctrl, command, output);
        
        double query_time = simple_timer_end(query_timer);
        free_simple_timer(query_timer);
        fclose(output);
        
        // Comparar resultado com esperado
        char expected_file[512];
        snprintf(expected_file, sizeof(expected_file), 
                 "%s/command%d_output.txt", config->expected_results_path, query_num);
        
        CompareResult* compare_result = compare_files(expected_file, output_path);
        
        int test_passed = 0;
        if (compare_result) {
            test_passed = (query_success == 0) && get_compare_result_files_match(compare_result);
        }
        
        // Imprimir resultado se houver discrepância
        if (!test_passed && compare_result) {
            print_compare_result(compare_result, query_num, output_path);
        }
        
        // Adicionar resultado às métricas
        add_execution_result(metrics, query_type, query_time, test_passed);
        
        // Libertar memória
        if (compare_result) {
            free_compare_result(compare_result);
        }
        
        query_num++;
    }
    
    fclose(input_file);
    
    double total_time = simple_timer_end(total_timer);
    set_program_metrics_total_time(metrics, total_time);
    free_simple_timer(total_timer);
    
    // Cleanup
    // controller_destroy(ctrl);
    // database_destroy(db);
    
    // Finalizar cálculos de métricas
    finalize_metrics(metrics);
    
    return metrics;
}