#include "../include/comparador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct CompareResult {
    int lines_correct;      // Número de linhas corretas
    int total_lines;        // Total de linhas comparadas
    int first_diff_line;    // Primeira linha com diferença (0 se iguais)
    int files_match;        // 1 se ficheiros são idênticos, 0 caso contrário
};

void free_compare_result(CompareResult* result) {
    free(result);
}

CompareResult* compare_files(const char* expected_file, const char* actual_file) {
    CompareResult* result = malloc(sizeof(struct CompareResult));
    if (!result) {
        return NULL;
    }
    
    result->lines_correct = 0;
    result->total_lines = 0;
    result->first_diff_line = 0;
    result->files_match = 0;
    
    FILE* expected = fopen(expected_file, "r");
    FILE* actual = fopen(actual_file, "r");
    
    if (!expected || !actual) {
        if (expected) fclose(expected);
        if (actual) fclose(actual);
        free_compare_result(result);
        return NULL; // Retorna NULL se não conseguir abrir ficheiros
    }
    
    char expected_line[1024], actual_line[1024];
    int line_number = 1;
    int files_match = 1;
    
    while (1) {
        char* exp_result = fgets(expected_line, sizeof(expected_line), expected);
        char* act_result = fgets(actual_line, sizeof(actual_line), actual);
        
        // Se ambos chegaram ao fim, terminou
        if (!exp_result && !act_result) {
            break;
        }
        
        // Se um terminou e outro não, ficheiros são diferentes
        if (!exp_result || !act_result) {
            if (files_match && result->first_diff_line == 0) {
                result->first_diff_line = line_number;
            }
            files_match = 0;
            break;
        }
        
        // Remove quebra de linha do final para comparação
        size_t exp_len = strlen(expected_line);
        size_t act_len = strlen(actual_line);
        
        if (exp_len > 0 && expected_line[exp_len - 1] == '\n') {
            expected_line[exp_len - 1] = '\0';
        }
        if (act_len > 0 && actual_line[act_len - 1] == '\n') {
            actual_line[act_len - 1] = '\0';
        }
        
        // Compara linhas
        if (strcmp(expected_line, actual_line) == 0) {
            result->lines_correct++;
        } else {
            if (files_match && result->first_diff_line == 0) {
                result->first_diff_line = line_number;
            }
            files_match = 0;
        }
        
        result->total_lines++;
        line_number++;
    }
    
    result->files_match = files_match;
    
    fclose(expected);
    fclose(actual);
    
    return result;
}

int get_query_type(const char* command) {
    if (!command || command[0] == '\0') {
        return 0;
    }
    
    // Remove espaços do início
    while (*command == ' ' || *command == '\t') {
        command++;
    }
    
    // Verifica se o primeiro caractere é um dígito
    if (command[0] >= '1' && command[0] <= '9') {
        return command[0] - '0';
    }
    
    return 0; // Query inválida
}

int get_compare_result_files_match(const CompareResult* result) {
    if (!result) {
        return 0;
    }
    return result->files_match;
}

void print_compare_result(const CompareResult* result, int query_num, const char* actual_file) {
    if (!result) {
        return;
    }
    
    if (result->files_match) {
        // Nao imprime nada para testes que passaram - sera contabilizado no relatorio final
    } else {
        if (result->first_diff_line > 0) {
            printf("Discrepancia na query %d: linha %d de \"%s\"\n", 
                   query_num, result->first_diff_line, actual_file);
        } else {
            printf("Erro ao comparar query %d: ficheiro \"%s\"\n", 
                   query_num, actual_file);
        }
    }
}