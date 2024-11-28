#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.h"
#include "functions.c"
#include <time.h>

int main(int argc, char* argv[]) {
    clock_t inicio, fim;
    inicio = clock();

    if (argc != 3) {
        fprintf(stderr, "Use: %s <arquivo_entrada.txt> <arquivo_saida.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* nome_arquivo = argv[1];
    int linhas, colunas;

    double** matriz = ler_matriz_aumentada(nome_arquivo, &linhas, &colunas);

    if (colunas != linhas + 1) {
        fprintf(stderr, "Erro: a matriz no arquivo não é uma matriz aumentada válida (%dx%d).\n", linhas, colunas);
        liberar_matriz(matriz, linhas);
        return EXIT_FAILURE;
    }


    double* solucao = eliminacao_gaussiana(matriz, linhas);

    escreve_matriz_arquivo(linhas, colunas, matriz, solucao, argv[2]);

    liberar_matriz(matriz, linhas);

    fim = clock();

    double tempo_total = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo total: %.6lf segundos\n", tempo_total);

    return 0;
}

