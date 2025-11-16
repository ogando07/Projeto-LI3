#include "../include/executor_testes.h"
#include "../include/metricas.h"
#include "../include/comparador.h"
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


int main(int argc, char* argv[]) {
    // Verificar argumentos
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <caminho_dataset> <ficheiro_comandos> <pasta_resultados_esperados>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s dataset-erros/ input.txt resultados-esperados/\n", argv[0]);
        return 1;
    }
    
    // Configurar teste
    TestConfig* config = create_test_config(argv[1], argv[2], argv[3], "resultados");
    
    if (!config) {
        fprintf(stderr, "Erro ao criar configuração de testes\n");
        return 1;
    }
    
    printf("=== PROGRAMA DE TESTES ===\n");
    printf("Dataset: %s\n", get_test_config_dataset_path(config));
    printf("Comandos: %s\n", get_test_config_input_file(config));
    printf("Resultados esperados: %s\n", get_test_config_expected_results_path(config));
    printf("\n");
    
    // Validar configuração
    if (!validate_config(config)) {
        fprintf(stderr, "Erro na configuração dos testes\n");
        free_test_config(config);
        return 1;
    }
    
    // Contar comandos para estatística
    int total_commands = count_commands(get_test_config_input_file(config));
    if (total_commands > 0) {
        printf("Total de comandos a executar: %d\n\n", total_commands);
    }
    
    // Executar todos os testes usando integração direta
    ProgramMetrics* metrics = execute_all_tests_integrated(config);
    
    if (!metrics) {
        fprintf(stderr, "Erro ao executar testes\n");
        return 1;
    }
    
    // Imprimir relatório final
    print_metrics_report(metrics);
    
    // Imprimir relatório final
    print_metrics_report(metrics);
    
    // Libertar memória
    free_program_metrics(metrics);
    free_test_config(config);
    
    return 0;
}